/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: EDA AFE
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Interface with custom electrodermal activity frontend 
 * for control and acquisition of voltage / current outputs
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

/*
 * Included files
 */

/* Standard C library includes */

/* SDK includes */

#define NRF_LOG_MODULE_NAME EDA
#define NRF_LOG_INFO_COLOR  3
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrfx_gpiote.h"
#include "nrfx_ppi.h"
#include "nrfx_saadc.h"
#include "nrfx_timer.h"
#include "nrfx_rtc.h"

/* Project includes */

#include "calendar/calendar.h"
#include "eda_cfg.h"
#include "eda_afe.h"

/*
 * Local constants
 */

#define EDA_CLK_FREQ                    EDA_SAMPLING_RATE           /**< EDA clock frequency in Hz */
#define SAADC_MAX_SAMPLES_NUMBER        (EDA_ADC_BUFFER_SIZE * 2)   /**< Number of samples in SAADC buffer. Contains both V and I samples */

/*
 * Local macros
 */

/*
 * Public variables
 */

/*
 * Local types
 */

/*
 * Local variables
 */

static nrfx_timer_t eda_clk_timer = NRFX_TIMER_INSTANCE(EDA_CLK_TIMER_INSTANCE);
static nrf_saadc_value_t saadc_buffer_pool[2][SAADC_MAX_SAMPLES_NUMBER];

static eda_buffer_t eda_buffer;
static eda_event_handler_t eda_event_handler = NULL;

/*
 * Local functions
 */

static void saadc_event_handler(nrfx_saadc_evt_t const *p_event);

/****************************************************************
 * IMPLEMENTATION
 ****************************************************************/

/*
 * Public functions
 */


/**
 * @brief Initialize pheripherals and start AFE control
 */
void EDA_Init(eda_event_handler_t event_handler)
{
    /* Store event handler for callbacks */
    eda_event_handler = event_handler;

    /* Initialize EDA clock pin for toggling task from timer using ppi */
    if (nrfx_gpiote_is_init() != true) {
        APP_ERROR_CHECK(nrfx_gpiote_init());
    }
    nrfx_gpiote_out_config_t eda_clk_pin_config = NRFX_GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
    APP_ERROR_CHECK(nrfx_gpiote_out_init(EDA_CLK_PIN, &eda_clk_pin_config));
    nrfx_gpiote_out_task_enable(EDA_CLK_PIN);

    /* Initialize adc for sampling voltage and current channels using ppi */
    nrfx_saadc_config_t saadc_config = NRFX_SAADC_DEFAULT_CONFIG;
    saadc_config.interrupt_priority = APP_IRQ_PRIORITY_HIGHEST;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_14BIT;
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_4X;
    APP_ERROR_CHECK(nrfx_saadc_init(&saadc_config, saadc_event_handler));

    /* Input range for both channels is (reference = +/- VDD/4) / (gain = 1/2) = +- VDD/2 */
    nrf_saadc_channel_config_t saadc_channel_config_v = {
        .pin_p      = EDA_VOUT_ANALOG_INPUT,
        .pin_n      = EDA_VREF_ANALOG_INPUT,
        .mode       = NRF_SAADC_MODE_DIFFERENTIAL,
        .gain       = NRF_SAADC_GAIN1_2,
        .reference  = NRF_SAADC_REFERENCE_VDD4,
        .burst      = NRF_SAADC_BURST_ENABLED,
        .acq_time   = NRF_SAADC_ACQTIME_3US,
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED
    };
    APP_ERROR_CHECK(nrfx_saadc_channel_init(0, &saadc_channel_config_v));
    nrf_saadc_channel_config_t saadc_channel_config_i = {
        .pin_p      = EDA_IOUT_ANALOG_INPUT,
        .pin_n      = EDA_VREF_ANALOG_INPUT,
        .mode       = NRF_SAADC_MODE_DIFFERENTIAL,
        .gain       = NRF_SAADC_GAIN1_2,
        .reference  = NRF_SAADC_REFERENCE_VDD4,
        .burst      = NRF_SAADC_BURST_ENABLED,
        .acq_time   = NRF_SAADC_ACQTIME_3US,
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED
    };
    APP_ERROR_CHECK(nrfx_saadc_channel_init(1, &saadc_channel_config_i));

    /* Preload double buffering */
    APP_ERROR_CHECK(nrfx_saadc_buffer_convert(saadc_buffer_pool[0], SAADC_MAX_SAMPLES_NUMBER));
    APP_ERROR_CHECK(nrfx_saadc_buffer_convert(saadc_buffer_pool[1], SAADC_MAX_SAMPLES_NUMBER));

    /* Set up PPI channel to connect timer event to pin task and adc task */
    static nrf_ppi_channel_t eda_clk_rtc_to_pin;
    uint32_t eda_clk_rtc_tick_event_addr = nrfx_rtc_event_address_get(CAL_GetRtcInstance(), NRF_RTC_EVENT_TICK);
    uint32_t eda_clk_pin_task_addr = nrfx_gpiote_out_task_addr_get(EDA_CLK_PIN);
    uint32_t adc_task_addr = nrfx_saadc_sample_task_get();
    APP_ERROR_CHECK(nrfx_ppi_channel_alloc(&eda_clk_rtc_to_pin));
    APP_ERROR_CHECK(nrfx_ppi_channel_assign(eda_clk_rtc_to_pin, eda_clk_rtc_tick_event_addr, eda_clk_pin_task_addr));
    APP_ERROR_CHECK(nrfx_ppi_channel_fork_assign(eda_clk_rtc_to_pin, adc_task_addr));
    APP_ERROR_CHECK(nrfx_ppi_channel_enable(eda_clk_rtc_to_pin));
}


/**
 * @brief Stop acquisition and deinit peripherals
 */
void EDA_Deinit(void)
{
    nrfx_timer_disable(&eda_clk_timer);
    nrfx_ppi_free_all();
    nrfx_saadc_uninit();
    nrfx_gpiote_out_uninit(EDA_CLK_PIN);
    nrfx_timer_uninit(&eda_clk_timer);
}

/*
 * Local functions
 */

static void saadc_event_handler(nrfx_saadc_evt_t const *p_event)
{
    if (p_event->type == NRFX_SAADC_EVT_DONE)
    {
        /* Reload next buffer (double buffering is internal) */
        APP_ERROR_CHECK(nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, SAADC_MAX_SAMPLES_NUMBER));
        /* Call callback with event */
        eda_buffer.length = p_event->data.done.size;
        eda_buffer.samples = p_event->data.done.p_buffer;
        if (eda_event_handler != NULL)
        {
            eda_event_handler(EDA_EVENT_BUFFER_FULL, &eda_buffer);
        }
    }
}

/* END OF FILE */