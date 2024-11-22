/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: CALENDAR
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Real time calendar functions
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

#ifndef CALENDAR_H
#define CALENDAR_H

/*
 * Included files
 */

/* Standard C library includes */
#include "stdint.h"

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
 * @brief Initialize the calendar
 */
void CAL_Init(void);

/**
 * @brief Release the calendar
 */
void CAL_Deinit(void);

/**
 * @brief Update calendar time with Unix timestamp
 * @param[in] timestamp containing a Unix epoch
 */
void CAL_SetTime(const uint64_t timestamp, const uint32_t us);

/**
 * @brief Return current calendar time with Unix timestamp
 * @param[out] timestamp a pointer provided to store the Unix epoch read
 * @param[out] us a pointer provided to store the microseconds elapsed
 */
void CAL_GetTime(uint64_t * p_timestamp, uint32_t * p_us);

#endif /* CALENDAR_H */

/* END OF FILE */