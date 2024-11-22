/****************************************************************
 * Project: RENFORCE
 * Module: SDK CONFIGURATION
 *
 *---------------------------------------------------------------
 * @brief SDK configuration overrides for RENFORCE EDA firmware
 *
 *---------------------------------------------------------------
 * Copyright (c) 2020 PIWIO
 ****************************************************************/

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

    // Enable logging using RTT
    #ifndef NRF_LOG_BACKEND_RTT_ENABLED
    #define NRF_LOG_BACKEND_RTT_ENABLED 1
    #endif

    // Disable logging using UART
    #ifndef NRF_LOG_BACKEND_UART_ENABLED
    #define NRF_LOG_BACKEND_UART_ENABLED 0
    #endif

    // NRF_SDH_BLE_SERVICE_CHANGED  - Include the Service Changed characteristic in the Attribute Table.
    #ifndef NRF_SDH_BLE_SERVICE_CHANGED
    #define NRF_SDH_BLE_SERVICE_CHANGED 1
    #endif
    
    // NRF_DFU_BLE_REQUIRES_BONDS  - Require bond with peer.
    #ifndef NRF_DFU_BLE_REQUIRES_BONDS
    #define NRF_DFU_BLE_REQUIRES_BONDS 0
    #endif

    // NRF_BL_DFU_ENTER_METHOD_BUTTON - Enter DFU mode on button press.
    #ifndef NRF_BL_DFU_ENTER_METHOD_BUTTON
    #define NRF_BL_DFU_ENTER_METHOD_BUTTON 0
    #endif

#endif /* APP_CONFIG_H_ */
