/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: BLUETOOTH
 *
 *---------------------------------------------------------------
 * @brief Bluetooth module source file
 *
 * Implements mandatory services and characteristics for a HRS
 * profile (GAP, DI, HRS, BAS and NRF BLE UART)
 * Provides convenient API to start/stop advertising, update GATT
 * attributes and manage bonded devices.
 *
 * Dependencies : None
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

 /*
 * Included files
 */

/* Standard C library includes */
#include <stdint.h>
#include <stdlib.h>

/* NRF SDK includes */

// Macro APP_TIMER_TICKS
#include "app_timer.h"

// Generic BLE includes
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"

// BLE services includes
#include "ble_dis.h"
#include "ble_bas.h"
#include "ble_nus.h"
#include "ble_dfu.h"

// Other NRF BLE includes
#include "nrf_ble_gatt.h"
#include "nrf_ble_lesc.h"
#include "nrf_ble_qwr.h"
#include "nrf_dfu_settings.h"

// Macro for bootloader skipping CRC check at reboot
#include "nrf_bootloader_info.h"

// Log console
#define NRF_LOG_MODULE_NAME BLE
#define NRF_LOG_INFO_COLOR  3
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

// Power management module for DFU reboot
#include "nrf_power.h"
#include "nrf_pwr_mgmt.h"

// Soft device
#include "nrf_sdh.h"

// Bonding handlings
#include "peer_manager.h"
#include "peer_manager_handler.h"

/* Application includes */
#include "bluetooth.h"

/*
 * Local constants
 */

#define DEVICE_NAME "RENFORCE EDA"                                          /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME "INL - INSA Lyon"                                 /**< Manufacturer. Will be passed to Device Information Service. */
#define HARDWARE_REV "0.2b"                                                 /**< Hardware version. Will be passed to Device Information Service */
#define FIRMWARE_REV xstr(APP_VERSION_STRING)                               /**< Firmware version. Will be passed to Device Information Service */
#define MODEL "EDA Spectrometer"                                            /**< Model name. Will be passed to Device Information Service */

#define APP_ADV_INTERVAL 500 //1000    /**< The advertising interval (in units of 0.625 ms). */
#define APP_ADV_DURATION 30000   /**< The advertising duration in units of 10 milliseconds. */

#define APP_BLE_CONN_CFG_TAG 1  /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(12, UNIT_1_25_MS) /**< Minimum acceptable connection interval (15 ms). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(24, UNIT_1_25_MS) /**< Maximum acceptable connection interval (30 ms). */
#define SLAVE_LATENCY 0                                    /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(200, UNIT_10_MS)   /**< Connection supervisory timeout (2 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(1500) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(1500)  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 1                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define LESC_DEBUG_MODE 0 /**< Set to 1 to use LESC debug keys, allows you to use a sniffer to inspect traffic. */

#define SEC_PARAM_BOND 1                               /**< Perform bonding. */
#define SEC_PARAM_MITM 0                               /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC 1                               /**< LE Secure Connections enabled. */
#define SEC_PARAM_KEYPRESS 0                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_NONE /**< No I/O capabilities. */
#define SEC_PARAM_OOB 0                                /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE 7                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE 16                      /**< Maximum encryption key size. */

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define NUS_SERVICE_UUID_TYPE BLE_UUID_TYPE_VENDOR_BEGIN /**< UUID type for the Nordic UART Service (vendor specific). */

/*
 * Local macros
 */
#define xstr(s) str(s)
#define str(s) #s

/*
 * Public variables
 */
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT); /**< Nordic uart service instance. */
BLE_BAS_DEF(m_bas);                               /**< Structure used to identify the battery service. */
NRF_BLE_GATT_DEF(m_gatt);                         /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                           /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);               /**< Advertising module instance. */

/*
 * Local types
 */

/*
 * Local variables
 */
static uint8_t      m_adv_mode = BLE_ADV_MODE_IDLE;             /**< Retain advertising mode */
static uint16_t     m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static pm_peer_id_t m_peer_id;                                  /**< Device reference handle to the current bonded central. */
static ble_uuid_t   m_adv_uuids[] =                             /**< Universally unique service identifiers. */
    {
        //{BLE_UUID_NUS_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN}
    };
static uint8_t dfu_request = 0;
static uint8_t m_mtu = 27 - OPCODE_LENGTH - HANDLE_LENGTH;
static uint8_t m_phy = BLE_GAP_PHY_1MBPS;

static ble_connection_callback_t    m_connection_event_callback = NULL;     /**< Pointer to user application callback for handling connection events */
static ble_advertising_callback_t   m_advertising_event_callback = NULL;    /**< Pointer to user application callback for handling advertising events */
static ble_uart_rx_callback_t       m_nus_event_callback = NULL;            /**< Pointer to user application callback for handling RX data from BLE UART */

static uint8_t  m_stop_adv_after_disconnect = 0;    /**< Store request for not starting advetising after next BLE disconnection */
static uint8_t  m_stop_adv_after_idle = 0;          /**< Prevent from restarting advertising after timeout */
static uint8_t  m_tx_busy = 0;                      /**< Store if the module is actually sending a buffer through TX nus */
static uint8_t  * m_tx_buffer = NULL;               /**< The buffer being sent */
static uint16_t m_tx_buffer_bytes_left = 0;         /**< The remaining number of bytes to be sent */ 
static uint16_t m_tx_buffer_size = 0;               /**< The total number of bytes to send */
static uint8_t  m_tx_notifications_enabled = 0;     /**< Store if remote suscribed to TX notifications */
static uint8_t  m_bas_notifications_enabled = 0;    /**< Store if remote suscribed to battery level notifications */

/*
 * Local functions
 */

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name);

static void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt);
static void pm_evt_handler(pm_evt_t const *p_evt);
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt);
static void on_adv_evt(ble_adv_evt_t ble_adv_evt);
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);
static void nus_data_handler(ble_nus_evt_t *p_evt);
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event);

static void ble_stack_init(void);
static void gap_params_init(void);
static void gatt_init(void);
static void advertising_init(void);
static void services_init(void);
static void conn_params_init(void);
static void peer_manager_init();
static void delete_bonds(void);
static void whitelist_set(pm_peer_id_list_skip_t skip);
static void disconnect(uint16_t conn_handle, void *p_context);

static void nus_send_next_packet(void);

static void on_bas_evt(ble_bas_t * p_bas, ble_bas_evt_t * p_evt);

/****************************************************************
 * IMPLEMENTATION
 ****************************************************************/

/*
 * Public functions
 */

/**
 * @brief Initialize GATT server
 */
void BLE_Init()
{
    NRF_LOG_INFO("Init");
    if (nrf_sdh_is_enabled() == false)
    {
    #if(NRF_DFU_BLE_BUTTONLESS_SUPPORTS_BONDS == 1)
        // Initialize the async SVCI interface to bootloader before any interrupts are enabled.
        APP_ERROR_CHECK(ble_dfu_buttonless_async_svci_init());
    #endif

        ble_stack_init();
        gap_params_init();
        gatt_init();
        // Peer manager init must be before services init in case of DFU
        peer_manager_init();
        // Services init must be before advertising init in case of advertising custom service UUIDS
        services_init();
        advertising_init();
        conn_params_init();
    }
}

/**
 * @brief Disable Bluetooth
 */
void BLE_Deinit()
{
    nrf_sdh_disable_request();
}

/**
 * @brief Return true if there is an active connection
 *
 */
bool BLE_IsConnected(void)
{
    return (m_conn_handle != BLE_CONN_HANDLE_INVALID);
}


/**
 * @brief Register a callback to be called when connection / disconnection event
 *
 */
void BLE_SetConnectionCallback(void * callback)
{
    m_connection_event_callback = callback;
}


/**
 * @brief Register a callback to be called when advertising event
 *
 */
void BLE_SetAdvertisingCallback(void * callback)
{
    m_advertising_event_callback = callback;
}

/**
 * @brief Disconnect from active connection
 *
 */
void BLE_Disconnect(void)
{
    ble_conn_state_for_each_connected(disconnect, NULL);
}

/**
 * @brief Function for starting advertising.
 */
void BLE_AdvertisingStart(bool erase_bonds)
{   
    m_stop_adv_after_idle = 1;
    m_stop_adv_after_disconnect = 0;

    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else
    {
        /* Uncomment this line if you want to secure connection with whitelisting */
        // whitelist_set(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR);

        APP_ERROR_CHECK(ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST));
    }
}

/**
 * @brief Function for stopping advertising (only if not connected).
 */
void BLE_AdvertisingStop(void)
{
    m_stop_adv_after_idle = 1;
    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        m_stop_adv_after_disconnect = 1;
    }
    else
    {
        if (m_adv_mode != BLE_ADV_MODE_IDLE)
        {
            APP_ERROR_CHECK(sd_ble_gap_adv_stop(m_advertising.adv_handle));
            //ble_advertising_start(&m_advertising, BLE_ADV_MODE_IDLE);
        }
    }
}

/**
 * @brief Function returning usable MTU
 */
uint8_t BLE_GetMtu(void)
{
    return m_mtu;
}

/**
 * @brief Function requesting 2M PHY layer
 */
void BLE_SetPhy2M(void)
{
    ret_code_t err_code;
    ble_gap_phys_t const phys =
    {
        .rx_phys = BLE_GAP_PHY_2MBPS,
        .tx_phys = BLE_GAP_PHY_2MBPS,
    };
    err_code = sd_ble_gap_phy_update(m_conn_handle, &phys);
    APP_ERROR_CHECK(err_code);   
}

/**
 * @brief Function returning PHY layer used
 */
uint8_t BLE_GetPhy(void)
{
    return m_phy;
}

/**
 * @brief Function for updating the Battery Level characteristic
 *        in Battery Service.
 */
void BLE_BatteryLevelUpdate(uint8_t battery_level)
{
    ret_code_t err_code;
    if (m_conn_handle != BLE_CONN_HANDLE_INVALID) {
        if (m_bas_notifications_enabled != 0) {
            err_code = ble_bas_battery_level_update(&m_bas, battery_level, m_conn_handle);
            if ((err_code != NRF_SUCCESS) &&
                (err_code != NRF_ERROR_INVALID_STATE) &&
                (err_code != NRF_ERROR_RESOURCES) &&
                (err_code != NRF_ERROR_BUSY) &&
                (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
            {
                APP_ERROR_HANDLER(err_code);
            }

            /* Force notification sending ! (workaround) */
            ble_gatts_value_t  gatts_value;
            memset(&gatts_value, 0, sizeof(gatts_value));
            gatts_value.len     = sizeof(uint8_t);
            gatts_value.offset  = 0;
            gatts_value.p_value = &battery_level;

            ble_gatts_hvx_params_t hvx_params;
            memset(&hvx_params, 0, sizeof(hvx_params));
            hvx_params.handle = m_bas.battery_level_handles.value_handle;
            hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset = gatts_value.offset;
            hvx_params.p_len  = &gatts_value.len;
            hvx_params.p_data = gatts_value.p_value;

            sd_ble_gatts_hvx(m_conn_handle, &hvx_params);
        }
    }

}

/**
 * @brief Function to assign the callback to be called when receiving data from
 *        BLE Nordic Uart Service
 */
void BLE_SetUartRXCallback(void *callback)
{
    m_nus_event_callback = callback;
}

/**
 * @brief Function to send data over Nordic Uart Service
 */
void BLE_UartSendArray(uint8_t *data, uint16_t length)
{
    /* Don't send data if notifications are not enabled */
    if (m_tx_notifications_enabled == 0)
    {
        return;
    }

    /* Don't send data if already sending another buffer */
    if (m_tx_busy != 0)
    {
        return;
    }

    /* Start sending the first packet, others will be sent automatically */
    m_tx_buffer = data;
    m_tx_buffer_bytes_left = length;
    m_tx_buffer_size = length;
    nus_send_next_packet();
}

/*
 * Local functions
 */

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for setting filtered whitelist.
 *
 * @param[in] skip  Filter passed to @ref pm_peer_id_list.
 */
static void whitelist_set(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
    uint32_t peer_id_count = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEBUG("m_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d",
                 peer_id_count + 1,
                 BLE_GAP_WHITELIST_ADDR_MAX_COUNT);

    err_code = pm_whitelist_set(peer_ids, peer_id_count);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for setting filtered device identities.
 *
 * @param[in] skip  Filter passed to @ref pm_peer_id_list.
 */
static void identities_set(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT];
    uint32_t peer_id_count = BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT;

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
    APP_ERROR_CHECK(err_code);

    err_code = pm_device_identities_list_set(peer_ids, peer_id_count);
    APP_ERROR_CHECK(err_code);
}

/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}

static void advertising_config_get(ble_adv_modes_config_t *p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout = APP_ADV_DURATION;
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    /* Automatically add 2 MSB of MAC address to the name */
    ble_gap_addr_t ble_addr;
    sd_ble_gap_addr_get(&ble_addr);
    char device_name[strlen(DEVICE_NAME)+7];
    sprintf(device_name, "%s %02X%02X", DEVICE_NAME, ble_addr.addr[1], ble_addr.addr[0]);
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (uint8_t *)device_name,
                                          strlen(device_name));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void services_init(void)
{
    ret_code_t err_code;
    ble_dis_init_t dis_init;
    ble_bas_init_t bas_init;
    ble_nus_init_t nus_init;
    ble_dfu_buttonless_init_t dfus_init;
    nrf_ble_qwr_init_t qwr_init;

    // Initialize Queued Write Module.
    memset(&qwr_init, 0, sizeof(qwr_init));
    qwr_init.error_handler = nrf_qwr_error_handler;
    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);

    // Setting app version
    /*nrf_dfu_settings_t *p_dfu_settings = (nrf_dfu_settings_t *)BOOTLOADER_SETTINGS_ADDRESS;
    char app[11];
    snprintf(app, 11, "%lu", p_dfu_settings->app_version);
    ble_srv_ascii_to_utf8(&dis_init.sw_rev_str, app);*/

    // Hardware revision
    ble_srv_ascii_to_utf8(&dis_init.hw_rev_str, (char *)HARDWARE_REV);

    // Firmware revision
    ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, (char *)FIRMWARE_REV);

    // Model name
    ble_srv_ascii_to_utf8(&dis_init.model_num_str, (char *)MODEL);

    // Here the sec level for the Device Information Service can be changed/increased.
    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Battery Service.
    memset(&bas_init, 0, sizeof(bas_init));

    bas_init.evt_handler = on_bas_evt;
    bas_init.support_notification = true;
    bas_init.p_report_ref = NULL;
    bas_init.initial_batt_level = 100;

    // Here the sec level for the Battery Service can be changed/increased.
    bas_init.bl_rd_sec = SEC_OPEN;
    bas_init.bl_cccd_wr_sec = SEC_OPEN;
    bas_init.bl_report_rd_sec = SEC_OPEN;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Nordic Uart Service.
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);

    // Initialize DFU Service.
    memset(&dfus_init, 0, sizeof(dfus_init));

    dfus_init.evt_handler = ble_dfu_evt_handler;

    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID; //m_hrs.hrm_handles.cccd_handle;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond = SEC_PARAM_BOND;
    sec_param.mitm = SEC_PARAM_MITM;
    sec_param.lesc = SEC_PARAM_LESC;
    sec_param.keypress = SEC_PARAM_KEYPRESS;
    sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob = SEC_PARAM_OOB;
    sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    ret_code_t err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = true;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids = m_adv_uuids;

    init.config.ble_adv_whitelist_enabled = false; //true;
    init.config.ble_adv_directed_high_duty_enabled = false;
    init.config.ble_adv_directed_enabled = false;
    init.config.ble_adv_directed_interval = 0;
    init.config.ble_adv_directed_timeout = 0;
    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout = APP_ADV_DURATION;
    init.config.ble_adv_slow_enabled = false;
    init.config.ble_adv_slow_interval = 0;
    init.config.ble_adv_slow_timeout = 0;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        uint32_t err_code;
    case NRF_PWR_MGMT_EVT_PREPARE_DFU:
        NRF_LOG_DEBUG("NRF_PWR_MGMT_EVT_PREPARE_DFU");
        // Device ready to enter DFU
        err_code = sd_softdevice_disable();
        APP_ERROR_CHECK(err_code);
        err_code = app_timer_stop_all();
        APP_ERROR_CHECK(err_code);
        break;

    case NRF_PWR_MGMT_EVT_PREPARE_RESET:
        NRF_LOG_DEBUG("NRF_PWR_MGMT_EVT_PREPARE_RESET");
        break;

    case NRF_PWR_MGMT_EVT_PREPARE_SYSOFF:
        NRF_LOG_DEBUG("NRF_PWR_MGMT_EVT_PREPARE_SYSOFF");
        // Prepare device to enter sysoff forever
        err_code = sd_softdevice_disable();
        APP_ERROR_CHECK(err_code);
        err_code = app_timer_stop_all();
        APP_ERROR_CHECK(err_code);
        break;

    case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
        NRF_LOG_DEBUG("NRF_PWR_MGMT_EVT_PREPARE_WAKEUP");
        // Prepare device to enter sysoff with wakeup source
        err_code = sd_softdevice_disable();
        APP_ERROR_CHECK(err_code);
        err_code = app_timer_stop_all();
        APP_ERROR_CHECK(err_code);
        break;

    default:
        return true;
    }
    return true;
}

/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);

static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void *p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        /* Goto Sysoff for reset in DFU mode only if requested from DFU service */
        if (dfu_request == 1)
        {
            NRF_LOG_DEBUG("Buttonless dfu sdh observer event NRF_SDH_EVT_STATE_DISABLED");
            dfu_request = 0;
            // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
            nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

            //Go to system off.
            nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
        }
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
    {
        .handler = buttonless_dfu_sdh_state_observer,
};

static void disconnect(uint16_t conn_handle, void *p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to trigger disconnection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
}

// YOUR_JOB: Update this code if you want to do anything given a DFU event (optional).
/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_DEBUG("Device is preparing to enter bootloader mode.");

            // Store DFU request for going into SysOff when disabling SDH
            dfu_request = 1;

            // Prevent device from advertising on disconnect.
            ble_adv_modes_config_t config;
            advertising_config_get(&config);
            config.ble_adv_on_disconnect_disabled = true;
            ble_advertising_modes_config_set(&m_advertising, &config);

            // Disconnect all other bonded devices that currently are connected.
            // This is required to receive a service changed indication
            // on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
            NRF_LOG_DEBUG("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_DEBUG("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const *p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
    case PM_EVT_PEERS_DELETE_SUCCEEDED:
        NRF_LOG_DEBUG("Peers successfully deleted, advert will restart");
        ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        break;

    case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        if (p_evt->params.peer_data_update_succeeded.flash_changed && (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING))
        {
            NRF_LOG_DEBUG("New Bond, add the peer to the whitelist if possible");
            // Note: You should check on what kind of white list policy your application should use.

            whitelist_set(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR);
        }
        break;

    default:
        break;
    }
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;
    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_FAST:
        NRF_LOG_DEBUG("Fast advertising");
        m_adv_mode = ble_adv_evt;
        break;

    case BLE_ADV_EVT_SLOW:
        NRF_LOG_DEBUG("Slow advertising");
        m_adv_mode = ble_adv_evt;
        break;

    case BLE_ADV_EVT_IDLE:
        NRF_LOG_DEBUG("Stop advertising");
        m_adv_mode = ble_adv_evt;
        if (m_stop_adv_after_idle == 0)
        {
            APP_ERROR_CHECK(ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST));
        }
        break;

    case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
        NRF_LOG_DEBUG("High Duty Directed advertising.");
        m_adv_mode = ble_adv_evt;
        break;

    case BLE_ADV_EVT_DIRECTED:
        NRF_LOG_DEBUG("Directed advertising.");
        m_adv_mode = ble_adv_evt;
        break;

    case BLE_ADV_EVT_FAST_WHITELIST:
        NRF_LOG_DEBUG("Fast advertising with whitelist.");
        m_adv_mode = ble_adv_evt;
        break;

    case BLE_ADV_EVT_SLOW_WHITELIST:
        NRF_LOG_DEBUG("Slow advertising with whitelist.");
        m_adv_mode = ble_adv_evt;
        break;

    case BLE_ADV_EVT_WHITELIST_REQUEST:
    {
        ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
        ble_gap_irk_t whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
        uint32_t addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
        uint32_t irk_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

        NRF_LOG_DEBUG("Whitelist request");
        err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt,
                                    whitelist_irks, &irk_cnt);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist",
                      addr_cnt, irk_cnt);

        // Set the correct identities list (no excluding peers with no Central Address Resolution).
        identities_set(PM_PEER_ID_LIST_SKIP_NO_IRK);

        // Apply the whitelist.
        err_code = ble_advertising_whitelist_reply(&m_advertising,
                                                   whitelist_addrs,
                                                   addr_cnt,
                                                   whitelist_irks,
                                                   irk_cnt);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_ADV_EVT_PEER_ADDR_REQUEST:
    {
        pm_peer_data_bonding_t peer_bonding_data;

        NRF_LOG_DEBUG("Peer adress request");
        // Only Give peer address if we have a handle to the bonded peer.
        if (m_peer_id != PM_PEER_ID_INVALID)
        {
            err_code = pm_peer_data_bonding_load(m_peer_id, &peer_bonding_data);
            if (err_code != NRF_ERROR_NOT_FOUND)
            {
                APP_ERROR_CHECK(err_code);

                // Manipulate identities to exclude peers with no Central Address Resolution.
                identities_set(PM_PEER_ID_LIST_SKIP_ALL);

                ble_gap_addr_t *p_peer_addr = &(peer_bonding_data.peer_ble_id.id_addr_info);
                err_code = ble_advertising_peer_addr_reply(&m_advertising, p_peer_addr);
                APP_ERROR_CHECK(err_code);
            }
        }
    }
    break;

    default:
        break;
    }

    if (m_advertising_event_callback != NULL)
    {
        m_advertising_event_callback(ble_adv_evt);
    }
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
    //ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        NRF_LOG_ERROR("Parameters update failed !");
        //err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        //APP_ERROR_CHECK(err_code);
    }
    else if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_SUCCEEDED)
    {
        NRF_LOG_DEBUG("Parameters successfully updated");  
    }
}

/**@brief GATT module event handler.
 */
static void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt)
{
    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
    {
        NRF_LOG_DEBUG("GATT ATT MTU on connection 0x%x changed to %d",
                     p_evt->conn_handle,
                     p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH);
        m_mtu = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t err_code;
    ble_gap_conn_params_t conn_params;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
        APP_ERROR_CHECK(err_code);
        memcpy(&conn_params, &(p_ble_evt->evt.gap_evt.params.connected.conn_params), sizeof(ble_gap_conn_params_t));
        NRF_LOG_INFO("Connected with param. %u, %u, %u, %u", conn_params.min_conn_interval, conn_params.max_conn_interval, conn_params.slave_latency, conn_params.conn_sup_timeout);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("Disconnected, reason 0x%02x",
                     p_ble_evt->evt.gap_evt.params.disconnected.reason);
        m_tx_notifications_enabled = 0;
        m_bas_notifications_enabled = 0;
        m_tx_busy = 0;
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        if (m_stop_adv_after_disconnect == 1)
        {
            sd_ble_gap_adv_stop(m_advertising.adv_handle);
        }
        break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("PHY update request");
        ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE:
    {
        ble_gap_evt_phy_update_t const *phys = &(p_ble_evt->evt.gap_evt.params.phy_update);
        NRF_LOG_INFO("PHY updated with status 0x%02x to tx: 0x%02x and rx: 0x%02x", phys->status, phys->tx_phy, phys->rx_phy);
        m_phy = phys->tx_phy;
    }
    break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        NRF_LOG_ERROR("ATT Client Timeout");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        NRF_LOG_ERROR("GATT Server Timeout");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        NRF_LOG_DEBUG("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
        break;

    case BLE_GAP_EVT_AUTH_KEY_REQUEST:
        NRF_LOG_DEBUG("BLE_GAP_EVT_AUTH_KEY_REQUEST");
        break;

    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
        NRF_LOG_DEBUG("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
        break;

    case BLE_GAP_EVT_AUTH_STATUS:
        NRF_LOG_DEBUG("BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
                     p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                     p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                     p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                     *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                     *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
        break;

    default:
        // No implementation needed.
        break;
    }

    if (m_connection_event_callback != NULL)
    {
        m_connection_event_callback(p_ble_evt->header.evt_id);
    }
}

/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 */
static void nus_data_handler(ble_nus_evt_t *p_evt)
{
    switch (p_evt->type)
    {
    case BLE_NUS_EVT_RX_DATA:
        NRF_LOG_DEBUG("NUS RX %d bytes received", p_evt->params.rx_data.length);
        break;
    case BLE_NUS_EVT_TX_RDY:
        if (m_tx_buffer_bytes_left > 0)
        {
            nus_send_next_packet();
        }
        break;
    case BLE_NUS_EVT_COMM_STARTED:
        NRF_LOG_DEBUG("NUS TX notifications enabled");
        m_tx_notifications_enabled = 1;
        break;
    case BLE_NUS_EVT_COMM_STOPPED:
        NRF_LOG_DEBUG("NUS TX notification disabled");
        m_tx_notifications_enabled = 0;
        m_tx_busy = 0;
        break;
    default:
        break;
    }

    if (m_nus_event_callback != NULL)
    {
        m_nus_event_callback(p_evt);
    }
}

static void nus_send_next_packet(void)
{
    ret_code_t err_code;
    uint16_t to_send;

    if (m_tx_buffer_bytes_left > m_mtu)
    {
        to_send = m_mtu;
    }
    else
    {
        to_send = m_tx_buffer_bytes_left;
    }

    err_code = ble_nus_data_send(&m_nus, &m_tx_buffer[m_tx_buffer_size - m_tx_buffer_bytes_left], &to_send, m_conn_handle);
    if ((err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_NOT_FOUND))
    {
        APP_ERROR_CHECK(err_code);
    }
    m_tx_buffer_bytes_left -= to_send;
    if (m_tx_buffer_bytes_left == 0)
    {
        m_tx_busy = 0;
        //NRF_LOG_RAW_INFO("t2\n");
    }
}

/**@brief Function for handling the Battery Service events.
 *
 * @details This function will be called for all Battery Service events which are passed to the
 |          application.
 *
 * @param[in] p_bas  Battery Service structure.
 * @param[in] p_evt  Event received from the Battery Service.
 */
static void on_bas_evt(ble_bas_t * p_bas, ble_bas_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_BAS_EVT_NOTIFICATION_ENABLED:
            m_bas_notifications_enabled = 1;
            break; // BLE_BAS_EVT_NOTIFICATION_ENABLED

        case BLE_BAS_EVT_NOTIFICATION_DISABLED:
            m_bas_notifications_enabled = 0;
            break; // BLE_BAS_EVT_NOTIFICATION_DISABLED

        default:
            // No implementation needed.
            break;
    }
}