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

#ifndef EDA_AFE_H
#define EDA_AFE_H

/*
 * Included files
 */

/* Standard C library includes */

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

/**
 * @brief Enumeration of events send to eda_event_handler
 */
typedef enum {
    EDA_EVENT_BUFFER_FULL = 0,
    EDA_MAX_EVENT_NUM
} eda_event_t;

/**
 * @brief Buffer of SAADC data sent to eda_event_handler
 */
typedef struct {
    int16_t * samples;
    uint16_t length;
} eda_buffer_t;

/**
 * @brief Callback format for eda events to be sent to application
 */
typedef void (*eda_event_handler_t)(eda_event_t eda_event, void * data);

/*
 * Public variables
 */

/*
 * Public functions
 */

/**
 * @brief Initialize pheripherals and start AFE control
 */
void EDA_Init(eda_event_handler_t event_handler);

/**
 * @brief Stop acquisition and deinit peripherals
 */
void EDA_Deinit(void);

#endif /* EDA_AFE_H */

/* END OF FILE */