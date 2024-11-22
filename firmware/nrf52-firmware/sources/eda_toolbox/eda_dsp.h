/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: EDA DSP
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Provide functions to calculate various outputs from 
 * raw voltage and input signals
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

#ifndef EDA_DSP_H
#define EDA_DSP_H

/*
 * Included files
 */

/* Standard C library includes */

/* SDK includes */

#include "arm_math.h"

/* Project includes */

#include "renforce.pb.h"

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
 * @brief Initialize FFT instance
 */
void EDA_DSP_Init(void);

/**
 * @brief Try to clear any remaining FPU interrupt drawing current
 */
void EDA_DSP_Deinit(void);

/**
 * @brief Compute impedance at specific frequencies from voltage and current raw data
 */
int EDA_DSP_GetImpedance(int16_t * raw_buffer, Impedance * out_array);

void FPU_IRQHandler(void);


#endif /* EDA_DSP_H */

/* END OF FILE */