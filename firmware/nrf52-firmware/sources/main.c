/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: Main file
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Main source file handling system state through a finite
 * state machine
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

/*
 * Included files
 */

/* Standard C library includes */

/* SDK includes */

#define NRF_LOG_MODULE_NAME MAIN
#define NRF_LOG_INFO_COLOR  3
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble_advertising.h"
#include "ble_gap.h"
#include "ble_nus.h"
#include "nrf_ble_lesc.h"
#include "nrf_pwr_mgmt.h"
#include "nrfx_gpiote.h"
#include "app_scheduler.h"
#include "app_timer.h"

/* Project includes */

#include "bluetooth/bluetooth.h"
#include "eda_toolbox/eda_cfg.h"
#include "eda_toolbox/eda_afe.h"
#include "eda_toolbox/eda_dsp.h"
#include "fuel_gauge/fuel_gauge.h"
#include "calendar/calendar.h"

/* Protobuf and COBS includes */
#include "nanocobs/cobs.h"
#include "nanopb/pb_encode.h"
#include "nanopb/pb_decode.h"
#include "renforce.pb.h"

/*
 * Local constants
 */

#define SCHEDULER_QUEUE_SIZE      4
#define SCHEDULER_DATA_SIZE       sizeof(scheduler_event_t)

#define BLE_TX_MAX_BUFFER_SIZE      32768           /**< Maximum payload for BLE messages sent - TO BE REPLACED WITH PROTOBUF MAX MESSAGE SIZE*/
#define BATT_TIMER_MS               60000           /**< Update interval of battery state of charge */
#define RGB_LED_TIMER_MS            500

/*
 * Local macros
 */

#define xstr(s) str(s)
#define str(s) #s

/*
 * Public variables
 */

/*
 * Local types
 */

typedef enum {
    FSM_STATE_SLEEP = 0,
    FSM_STATE_ADVERT,
    FSM_STATE_PAIRING,
    FSM_STATE_CONNECTED,
    FSM_STATE_DISCONNECTED,
    FSM_STATE_SENDING,
    FSM_STATE_NUM
} fsm_state_t;

typedef enum {
    SCHEDULER_EVENT_DUMMY = 0,
    SCHEDULER_EVENT_CONNECTED,
    SCHEDULER_EVENT_DISCONNECTED,
    SCHEDULER_EVENT_ADV_STOP,
    SCHEDULER_EVENT_EDA_BUFFER_FULL,
    SCHEDULER_EVENT_NUM
} scheduler_event_type_t;

typedef struct {
    scheduler_event_type_t type;
    void * data;
} scheduler_event_t;

/*
 * Local variables
 */

static fsm_state_t fsm_state;
static scheduler_event_t scheduler_event;
static EdaBuffer edaBuffer = {
    .has_timestamp = true,
};
static uint8_t ble_tx_packet[EdaBuffer_size + 2];       /**< Maximum protobuf message size plus 2 COBS sentinel values */

static uint8_t pb_message[Timestamp_size + 1];
static Timestamp timestamp;

/*
 * Local functions
 */

static void ble_connection_event_handler(uint16_t ble_event);
static void ble_advertising_event_handler(ble_adv_evt_t ble_adv_evt);
static void ble_uart_rx_data_handler(ble_nus_evt_t *p_evt);

static void pb_decode_message(const uint8_t *p_data, uint16_t length);

static void eda_event_handler(eda_event_t eda_event, void * data);
static void eda_send_fft(eda_buffer_t * buffer);

static void rgb_led_init(void);
static void rgb_led_set(bool red, bool green, bool blue);
static void rgb_led_blink_blue(void);

APP_TIMER_DEF(rgb_led_timer_id);
static void rgb_led_timer_handler(void *p_context);

APP_TIMER_DEF(batt_timer_id);
static void batt_timer_handler(void *p_context);

static void scheduler_event_handler(void * p_event, uint16_t size);

/****************************************************************
 * IMPLEMENTATION
 ****************************************************************/

/*
 * Public functions
 */

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Initialize log module */
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    #ifdef APP_VERSION_STRING
        NRF_LOG_INFO("EDA Firmware version %s\n", xstr(APP_VERSION_STRING));
    #endif

    /* Initialize timer module */
    APP_ERROR_CHECK(app_timer_init());

    /* Initialize power management module */
    APP_ERROR_CHECK(nrf_pwr_mgmt_init());

    /* Initialize scheduler */
    APP_SCHED_INIT(SCHEDULER_DATA_SIZE, SCHEDULER_QUEUE_SIZE);

    /* Initialize RGB Led */
    app_timer_create(&rgb_led_timer_id, APP_TIMER_MODE_REPEATED, rgb_led_timer_handler);
    rgb_led_init();
    rgb_led_blink_blue();

    /* Start BLE stack */
    BLE_Init();
    BLE_SetConnectionCallback(ble_connection_event_handler);
    BLE_SetAdvertisingCallback(ble_advertising_event_handler);
    BLE_SetUartRXCallback(ble_uart_rx_data_handler);

    /* Start in advertising state */
    fsm_state = FSM_STATE_ADVERT;
    BLE_AdvertisingStart(false);

    /* Start frontend */
    EDA_DSP_Init();
    EDA_Init(eda_event_handler);

    /* Start calendar */
    CAL_Init();

    /* Start Fuel Gauge */
    FGA_Init();
    batt_timer_handler(NULL);
    app_timer_create(&batt_timer_id, APP_TIMER_MODE_REPEATED, batt_timer_handler);
    app_timer_start(batt_timer_id, APP_TIMER_TICKS(BATT_TIMER_MS), NULL);
    
    /* Enter main loop. */
    for (;;)
    {

        /* Handle BLE stack events */
        APP_ERROR_CHECK(nrf_ble_lesc_request_handler());

        /* Handle scheduler events */
        app_sched_execute();

        /* Put the system in the lowest power mode reachable */
        if (NRF_LOG_PROCESS() == false)
        {
            nrf_pwr_mgmt_run();
        }
    }
}

/*
 * Local functions
 */

static void ble_connection_event_handler(uint16_t ble_event)
{
    scheduler_event.type = SCHEDULER_EVENT_DUMMY;

    switch (ble_event)
    {
        case BLE_GAP_EVT_CONNECTED:
            scheduler_event.type = SCHEDULER_EVENT_CONNECTED;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            scheduler_event.type = SCHEDULER_EVENT_DISCONNECTED;
            break;

        default:
            break;
    }

    app_sched_event_put(&scheduler_event, sizeof(scheduler_event_t), scheduler_event_handler);
}

static void ble_advertising_event_handler(ble_adv_evt_t ble_adv_evt)
{
    scheduler_event.type = SCHEDULER_EVENT_DUMMY;

    switch(ble_adv_evt)
    {
        case BLE_ADV_EVT_IDLE:
            scheduler_event.type = SCHEDULER_EVENT_ADV_STOP;
            break;

        default:
            break;
    }

    app_sched_event_put(&scheduler_event, sizeof(scheduler_event_t), scheduler_event_handler);
}

static void ble_uart_rx_data_handler(ble_nus_evt_t *p_evt)
{
    switch (p_evt->type) 
    {
        case BLE_NUS_EVT_COMM_STARTED:
            rgb_led_set(false, true, false);
            break;
         
        case BLE_NUS_EVT_COMM_STOPPED:
            rgb_led_set(false, false, true);
            break;

        case BLE_NUS_EVT_RX_DATA:
            pb_decode_message(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);
            break;
        
        default:
            break;
    }
}

static void pb_decode_message(const uint8_t *p_data, uint16_t length)
{
    if (length > sizeof(pb_message))
    {
        NRF_LOG_ERROR("Size of message is %u but max is %u", length, Timestamp_size + 1);
        return;
    }
    memcpy(pb_message, p_data, length);
    /* COBS decode */
    cobs_ret_t cobs_ret = cobs_decode_inplace(pb_message, length);
    if (cobs_ret != COBS_RET_SUCCESS)
    {
        NRF_LOG_ERROR("error %d while decoding cobs", cobs_ret);
        return;
    }

    /* Decode protobuf message (should be a request) */
    pb_istream_t istream = pb_istream_from_buffer(pb_message + 1, length - 2);
    bool status = pb_decode(&istream, Timestamp_fields, &timestamp);
    if (!status) {
        NRF_LOG_ERROR("protobuf decoding failed: %s\n", PB_GET_ERROR(&istream));
        return;
    }

    CAL_SetTime(timestamp.time, timestamp.us);
}

static void eda_event_handler(eda_event_t eda_event, void * data)
{
    scheduler_event.type = SCHEDULER_EVENT_EDA_BUFFER_FULL;
    scheduler_event.data = data;
    app_sched_event_put(&scheduler_event, sizeof(scheduler_event_t), scheduler_event_handler);
}

static void scheduler_event_handler(void * p_event, uint16_t size)
{
    scheduler_event_t * event = (scheduler_event_t *)p_event;

    switch(event->type)
    {
        case SCHEDULER_EVENT_CONNECTED:
            NRF_LOG_INFO("CONNECTED");
            fsm_state = FSM_STATE_CONNECTED;
            rgb_led_set(false, false, true);
            break;

        case SCHEDULER_EVENT_DISCONNECTED:
            NRF_LOG_INFO("DISCONNECTED");
            fsm_state = FSM_STATE_ADVERT;
            rgb_led_blink_blue();
            break;

        case SCHEDULER_EVENT_ADV_STOP:
            NRF_LOG_INFO("ADV STOP");
            fsm_state = FSM_STATE_ADVERT;
            BLE_AdvertisingStop();
            BLE_AdvertisingStart(false);
            break;

        case SCHEDULER_EVENT_EDA_BUFFER_FULL:
            eda_send_fft(event->data);
            break;

        default:
            break;
    }
}

static void eda_send_fft(eda_buffer_t * buffer)
{
    //NRF_LOG_RAW_INFO("t1");
    int ret = EDA_DSP_GetImpedance(buffer->samples, edaBuffer.data);
    if (ret != 0) {
        return;
    }
    CAL_GetTime(&(edaBuffer.timestamp.time), &(edaBuffer.timestamp.us));
    //NRF_LOG_INFO("%llu.%06lu", edaBuffer.timestamp.time, edaBuffer.timestamp.us);
    pb_ostream_t ostream = pb_ostream_from_buffer(ble_tx_packet + 1, EdaBuffer_size);
    bool pb_ret = pb_encode(&ostream, EdaBuffer_fields, &edaBuffer);
    if (pb_ret == false) {
        NRF_LOG_ERROR("Error while encoding protobuf : %s", ostream.errmsg);
            return;
    }
    ble_tx_packet[0] = COBS_INPLACE_SENTINEL_VALUE;
    ble_tx_packet[ostream.bytes_written + 1] = COBS_INPLACE_SENTINEL_VALUE;
    cobs_ret_t cobs_ret = cobs_encode_inplace(ble_tx_packet, ostream.bytes_written + 2);
    // check for cobs_ret value
    if (cobs_ret != COBS_RET_SUCCESS) {
        NRF_LOG_ERROR("Error while encoding COBS message (err %u)", cobs_ret);
            return;
    }
    BLE_UartSendArray((uint8_t *)ble_tx_packet, ostream.bytes_written + 2);
}

static void rgb_led_init(void)
{
    if (nrfx_gpiote_is_init() != true) {
        APP_ERROR_CHECK(nrfx_gpiote_init());
    }
    nrfx_gpiote_out_config_t rgb_led_pin_config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
    APP_ERROR_CHECK(nrfx_gpiote_out_init(RGB_LED_RED_PIN, &rgb_led_pin_config));
    APP_ERROR_CHECK(nrfx_gpiote_out_init(RGB_LED_GREEN_PIN, &rgb_led_pin_config));
    APP_ERROR_CHECK(nrfx_gpiote_out_init(RGB_LED_BLUE_PIN, &rgb_led_pin_config));
}

static void rgb_led_set(bool red, bool green, bool blue)
{
    app_timer_stop(rgb_led_timer_id);
    if (red)   nrfx_gpiote_out_clear(RGB_LED_RED_PIN);   else nrfx_gpiote_out_set(RGB_LED_RED_PIN);
    if (green) nrfx_gpiote_out_clear(RGB_LED_GREEN_PIN); else nrfx_gpiote_out_set(RGB_LED_GREEN_PIN);
    if (blue)  nrfx_gpiote_out_clear(RGB_LED_BLUE_PIN);  else nrfx_gpiote_out_set(RGB_LED_BLUE_PIN);
}

static void rgb_led_blink_blue(void)
{
    nrfx_gpiote_out_set(RGB_LED_RED_PIN);
    nrfx_gpiote_out_set(RGB_LED_GREEN_PIN);
    nrfx_gpiote_out_clear(RGB_LED_BLUE_PIN);
    app_timer_start(rgb_led_timer_id, APP_TIMER_TICKS(RGB_LED_TIMER_MS), NULL);
}

static void rgb_led_timer_handler(void *p_context)
{
    nrfx_gpiote_out_toggle(RGB_LED_BLUE_PIN);
}

static void batt_timer_handler(void *p_context)
{
    uint8_t soc = FGA_GetStateOfCharge();
    BLE_BatteryLevelUpdate(soc);
    NRF_LOG_INFO("Batt state %u %%", soc);
    if (soc < 20) {
        rgb_led_set(true, false, false);
    }
}


/* END OF FILE */