/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: EDA CFG
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Contains some global definitions for the whole toolbox
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

#ifndef EDA_CFG_H
#define EDA_CFG_H

/*
 * Included files
 */

/* Standard C library includes */

/* SDK includes */

/* Project includes */

#include "idac_array.h"

/*
 * Public constants
 */

#define EDA_TIA_RESISTANCE          220000.0f                                   /**< value of resistance connected to the TIA to perform current range scaling */

#define EDA_VOLTAGE_SCALE           (3.0f/16384.0f)                             /**< range is +/- VDD/2 for signed 14 bits = (1.65f / 8192.0f) */
#define EDA_CURRENT_SCALE           (-EDA_VOLTAGE_SCALE/EDA_TIA_RESISTANCE)     /**< range is same than voltage but opposite and divided by TIA resistance */

#define USE_WAVEFORM                0                                           /**< if using theoretical current values instead of measured values*/
#define EDA_CURRENT_RESOLUTION      37.5e-9f                                    /**< as defined in the AFE, for use with theoretical current waveform */
extern const int8_t i_waveform[4096];                                           /**< the current waveform itself */

#define FFT_CPU_SIZE                1024
#define FFT_BIN_RATIO               4
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

#endif /* EDA_CFG_H */

/* END OF FILE */