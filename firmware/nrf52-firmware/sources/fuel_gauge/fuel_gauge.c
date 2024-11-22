/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: FUEL GAUGE
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Provide functions to support BQ27441 fuel gauge
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

/*
 * Included files
 */

/* Standard C library includes */

/* SDK includes */
#define NRF_LOG_MODULE_NAME FGA
#define NRF_LOG_INFO_COLOR  3
#include <nrf_log.h>
NRF_LOG_MODULE_REGISTER();
#include <nrf_drv_twi.h>

/* Project includes */
#include "bq27441/bq27441.h"

/*
 * Local constants
 */

static const nrf_drv_twi_t nrf_drv_twi = NRF_DRV_TWI_INSTANCE(0);

static const nrf_drv_twi_config_t nrf_drv_twi_config = {
    .frequency          = NRF_DRV_TWI_FREQ_400K,
    .scl                = FGA_I2C_SCL_PIN,
    .sda                = FGA_I2C_SDA_PIN,
    .interrupt_priority = TWI_DEFAULT_CONFIG_IRQ_PRIORITY,
    .hold_bus_uninit    = TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT,
    .clear_bus_init     = TWI_DEFAULT_CONFIG_CLR_BUS_INIT
};

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
 * Local functions
 */

static void twi_init(void);
static int16_t twi_write_mem(uint8_t DevAddress, uint8_t subAddress, uint8_t * src, uint8_t count);
static int16_t twi_read_mem(uint8_t DevAddress, uint8_t subAddress, uint8_t * dest, uint8_t count);

/*
 * Local variables
 */

static BQ27441_ctx_t BQ27441_ctx = {
    .BQ27441_i2c_address = BQ72441_I2C_ADDRESS,
    .read_reg = twi_read_mem,
    .write_reg = twi_write_mem
};

static bool is_init = false;

/****************************************************************
 * IMPLEMENTATION
 ****************************************************************/

/*
 * Public functions
 */

/**
 * @brief Initialize Fuel Gauge
 */
bool FGA_Init(void)
{
    twi_init();
    if (BQ27441_init(&BQ27441_ctx)) {
        BQ27441_setCapacity(155);
        BQ27441_setDesignEnergy(570);
        BQ27441_setTerminateVoltageMin(3300);
        BQ27441_setChargeVChgTermination(4200);
        is_init = true;
        NRF_LOG_INFO("BQ27441 initialized");
    }
    else {
        NRF_LOG_ERROR("Unable to initialize BQ27441");
    }
    return is_init;
}

/**
 * @brief Retrieve state of charge
 * @return Battery state of charge in percent
 */
uint8_t FGA_GetStateOfCharge(void)
{
    if (!is_init) return 0;
    NRF_LOG_INFO("Batt cur. is %d mA, volt. is %d V", BQ27441_current(AVG), BQ27441_voltage());
    return BQ27441_soc(UNFILTERED);
}

/*
 * Local functions
 */

static void twi_init(void)
{
    APP_ERROR_CHECK(nrf_drv_twi_init(&nrf_drv_twi, &nrf_drv_twi_config, NULL, NULL));
    nrf_drv_twi_enable(&nrf_drv_twi);
}

static int16_t twi_write_mem(uint8_t DevAddress, uint8_t subAddress, uint8_t * src, uint8_t count)
{
    uint8_t data[255];
    data[0] = subAddress;
    int i = 0;
    for (i = 0; i < count; i++) {
        data[i+1] = src[i];
    }
    //nrf_drv_twi_xfer_desc_t nrf_drv_twi_xfer_desc = NRFX_TWI_XFER_DESC_TXTX(DevAddress, &subAddress, 1, src, count);
    nrf_drv_twi_xfer_desc_t nrf_drv_twi_xfer_desc = NRFX_TWI_XFER_DESC_TX(DevAddress, data, count+1);
    ret_code_t ret = nrf_drv_twi_xfer(&nrf_drv_twi, &nrf_drv_twi_xfer_desc, 0);
    if (ret != NRF_SUCCESS) {
        NRF_LOG_ERROR("twi write transfer error %u", ret);
        return false;
    }
    return true;
    /*if (HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(DevAddress << 1), subAddress, 1, src, count, 50) == HAL_OK)
        return true;
    else
        return false;*/
}

static int16_t twi_read_mem(uint8_t DevAddress, uint8_t subAddress, uint8_t * dest, uint8_t count)
{
    nrf_drv_twi_xfer_desc_t nrf_drv_twi_xfer_desc = NRFX_TWI_XFER_DESC_TXRX(DevAddress, &subAddress, 1, dest, count);
    ret_code_t ret = nrf_drv_twi_xfer(&nrf_drv_twi, &nrf_drv_twi_xfer_desc, 0);
    if (ret != NRF_SUCCESS) {
        NRF_LOG_ERROR("twi read transfer error %u", ret);
        return false;
    }
    return true;

    /*if (HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(DevAddress << 1), subAddress, 1, dest, count, 50) == HAL_OK)
        return true;
    else
        return false;*/
}

/* END OF FILE */