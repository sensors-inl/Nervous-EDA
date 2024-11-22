/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: SDK CONFIGURATION
 *
 *---------------------------------------------------------------
 * @brief SDK configuration overrides for  firmware
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/*** ALTERNATIVE DEVKIT CONFIGURATION ***/

/*** LOGGER ***/

#define NRF_LOG_ENABLED                         1
#define NRF_LOG_BACKEND_RTT_ENABLED             1
#define NRF_LOG_BACKEND_RTT_TEMP_BUFFER_SIZE    128
#define NRF_LOG_DEFAULT_LEVEL                   4       /**< 0 => Off; 1 => Error; 2 => Warning; 3 => Info; 4 => Debug */
#define NRF_LOG_USES_COLORS                     1
#define NRF_LOG_BUFSIZE                         4096

/*** BLUETOOTH ***/

#define NRF_SDH_BLE_VS_UUID_COUNT               2
#define BLE_NUS_ENABLED                         1
#define BLE_DFU_ENABLED                         1
#define BLE_BAS_ENABLED                         1
#define BLE_DIS_ENABLED                         1
#define NRF_SDH_BLE_SERVICE_CHANGED             1
#define NRF_DFU_BLE_BUTTONLESS_SUPPORTS_BONDS   0
#define NRF_PWR_MGMT_CONFIG_AUTO_SHUTDOWN_RETRY 1
// Seems to be increased from 1408 (default) for additionnal characteristics
// WARNING : SD RAM size must be increased in linker size by same amount
#define NRF_SDH_BLE_GATTS_ATTR_TAB_SIZE         1440 //1408

/*** APPLICATION MODULE USED ***/

#define NRF_PWR_MGMT_ENABLED                    1
#define APP_SCHEDULER_ENABLED                   1
#define APP_TIMER_ENABLED                       1
#define GPIOTE_ENABLED                          1
#define PPI_ENABLED                             1
#define PWM_ENABLED                             1
#define SPI_ENABLED                             0
#define RTC_ENABLED                             1
#define SAADC_ENABLED                           1
#define TIMER_ENABLED                           1
#define TWI_ENABLED                             1

/*** APP TIMER ***/
#define APP_TIMER_CONFIG_RTC_FREQUENCY          0       /**< 0 => 32768 Hz; 1 => 16384 Hz; 3 => 8192 Hz; 7 => 4096 Hz; 15 => 2048 Hz; 31 => 1024 Hz */
#define APP_TIMER_CONFIG_IRQ_PRIORITY           6       /**< 2 (highest), 3, 6 or 7 (lowest) app priorities */
#define APP_TIMER_CONFIG_OP_QUEUE_SIZE          100

/*** RGB LED CONFIGURATION ***/
#define PWM0_ENABLED                            1
#define RGB_PWM_INSTANCE                        0       /**< PWM instance index */

/*** EDA AFE CONFIGURATION ***/
#define TIMER1_ENABLED                          1
#define EDA_CLK_TIMER_INSTANCE                  1

/*** FUEL GAUGE ***/
#define TWI0_ENABLED                            1
#define TWI0_USE_EASY_DMA                       1

/*** CALENDAR CONFIGURATION ***/
#define RTC2_ENABLED                            1
#define CAL_RTC_INSTANCE                        2       /**< Calendar RTC instance index. */

/*** BOARD GPIOS ***/
#define RGB_LED_RED_PIN                         10                      /**< Active Low */
#define RGB_LED_GREEN_PIN                       28                      /**< Active Low */
#define RGB_LED_BLUE_PIN                        9                       /**< Active Low */
#define EDA_CLK_PIN                             6
#define EDA_VOUT_ANALOG_INPUT                   NRF_SAADC_INPUT_AIN1    /**< P0.03 */
#define EDA_IOUT_ANALOG_INPUT                   NRF_SAADC_INPUT_AIN3    /**< P0.05 */
#define EDA_VREF_ANALOG_INPUT                   NRF_SAADC_INPUT_AIN0    /**< P0.02 */
#define FGA_I2C_SDA_PIN                         31
#define FGA_I2C_SCL_PIN                         30

#endif /* APP_CONFIG_H_ */
