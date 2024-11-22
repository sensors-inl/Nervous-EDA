/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: BLUETOOTH
 *
 *---------------------------------------------------------------
 * @brief Bluetooth module header file
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

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

/*
 * Included files
 */

#include <stdint.h>
#include "ble_advertising.h"
#include "ble_nus.h"

/*
 * Public constants
 */

/*
 * Public macros
 */

/*
 * Public types
 */

/**@brief Connection event callback type */
typedef void (*ble_connection_callback_t)(uint16_t ble_event);

/**@brief Advertising event callback type */
typedef void (*ble_advertising_callback_t)(ble_adv_evt_t ble_adv_evt);

/**@brief Nordic Uart Service RX data callback type */
typedef void (*ble_uart_rx_callback_t)(ble_nus_evt_t *p_evt);

/*
 * Public variables
 */

/*
 * Public functions
 */

/**
 * @brief Initialise GATT server according to Nordic example
 */
void BLE_Init();

/**
 * @brief Disable Bluetooth
 */
void BLE_Deinit();

/**
 * @brief Start BLE advertising
 *
 * @param[in] erase_bonds is a boolean value to erase previous peers bonded
 */
void BLE_AdvertisingStart(bool erase_bonds);

/**
 * @brief Stop BLE advertising
 *
 */
void BLE_AdvertisingStop(void);

/**
 * @brief Function returning usable MTU
 */
uint8_t BLE_GetMtu(void);

/**
 * @brief Function requesting 2M PHY layer
 */
void BLE_SetPhy2M(void);

/**
 * @brief Function returning PHY layer used
 */
uint8_t BLE_GetPhy(void);

/**
 * @brief Return true if there is an active connection
 *
 */
bool BLE_IsConnected(void);

/**
 * @brief Register a callback to be called when connection / disconnection event
 *
 */
void BLE_SetConnectionCallback(void * callback);

/**
 * @brief Register a callback to be called when advertising event
 *
 */
void BLE_SetAdvertisingCallback(void * callback);

/**
 * @brief Disconnect from active connection
 *
 */
void BLE_Disconnect(void);

/**
 * @brief Function for updating the Battery Level characteristic
 *        in Battery Service.
 * @param[in] battery_level is the battery level in percent
 */
void BLE_BatteryLevelUpdate(uint8_t battery_level);

/**
 * @brief Function to assign the callback to be called when receiving data from
 *        BLE Nordic Uart Service
 */
void BLE_SetUartRXCallback(void *callback);

/**
 * @brief Function to send data over Nordic Uart Service
 */
void BLE_UartSendArray(uint8_t *data, uint16_t length);

#endif /* BLUETOOTH_H_ */
