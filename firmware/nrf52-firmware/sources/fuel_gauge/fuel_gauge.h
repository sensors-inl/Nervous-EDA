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

#ifndef FUEL_GAUGE_H
#define FUEL_GAUGE_H


/*
 * Included files
 */

/* Standard C library includes */
#include <stdint.h>

/* SDK includes */

/* Project includes */

/*
 * Public constants
 */

/*
 * Public macros
 */

/*
 * Public types
 */

/*
 * Public variables
 */

/*
 * Public functions
 */

/**
 * @brief Initialize Fuel Gauge
 * @return true if successful
 */
bool FGA_Init(void);

/**
 * @brief Retrieve state of charge
 * @return Battery state of charge in percent
 */
uint8_t FGA_GetStateOfCharge(void);

#endif /* FUEL_GAUGE_H */

/* END OF FILE */