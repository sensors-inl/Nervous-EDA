/***************************************************************************//**
* \file IDAC7_Sink.h
* \version 1.10
*
* \brief
*  This file provides constants and parameter values for the IDAC7
*  component.
*
********************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_IDAC7_IDAC7_Sink_H)
#define CY_IDAC7_IDAC7_Sink_H

#include "cyfitter.h"
#include "CyLib.h"


/***************************************
*   Conditional Compilation Parameters
****************************************/

#define IDAC7_Sink_CURRENT_VALUE        ((uint32)0)
#define IDAC7_Sink_CURRENT_RANGE        ((uint32)4)
#define IDAC7_Sink_CURRENT_POLARITY     ((uint32)1)


/**************************************
*    Enumerated Types and Parameters
**************************************/

/* IDAC7 polarity setting constants */
#define IDAC7_Sink__POL_SOURCE 0
#define IDAC7_Sink__POL_SINK 1


/* IDAC7 range setting constants */
#define IDAC7_Sink__RNG_4_76UA 4
#define IDAC7_Sink__RNG_9_52UA 12
#define IDAC7_Sink__RNG_38_1UA 5
#define IDAC7_Sink__RNG_76_2UA 13
#define IDAC7_Sink__RNG_304_8UA 6
#define IDAC7_Sink__RNG_609_6UA 14
#define IDAC7_Sink__RNG_152_4UA 7


/* IDAC7 polarity setting definitions */
#define IDAC7_Sink_POL_SOURCE     ((uint32)IDAC7_Sink__POL_SOURCE)
#define IDAC7_Sink_POL_SINK       ((uint32)IDAC7_Sink__POL_SINK)

/* IDAC7 range setting definitions */
#define IDAC7_Sink_RNG_4_76UA     ((uint32)IDAC7_Sink__RNG_4_76UA)
#define IDAC7_Sink_RNG_9_52UA     ((uint32)IDAC7_Sink__RNG_9_52UA)
#define IDAC7_Sink_RNG_38_1UA     ((uint32)IDAC7_Sink__RNG_38_1UA)
#define IDAC7_Sink_RNG_76_2UA     ((uint32)IDAC7_Sink__RNG_76_2UA)
#define IDAC7_Sink_RNG_152_4UA    ((uint32)IDAC7_Sink__RNG_152_4UA)
#define IDAC7_Sink_RNG_304_8UA    ((uint32)IDAC7_Sink__RNG_304_8UA)
#define IDAC7_Sink_RNG_609_6UA    ((uint32)IDAC7_Sink__RNG_609_6UA)


/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
void IDAC7_Sink_Init(void);
void IDAC7_Sink_Enable(void);
void IDAC7_Sink_Start(void);
void IDAC7_Sink_Stop(void);
void IDAC7_Sink_SetValue(uint32 current);
void IDAC7_Sink_SetPolarity(uint32 polarity);
void IDAC7_Sink_SetRange(uint32 range);
/** @} group_general */

/**
* \addtogroup group_power
* @{
*/
void IDAC7_Sink_Sleep(void);
void IDAC7_Sink_Wakeup(void);
/** @} power */


/***************************************
*           Global Variables
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint32 IDAC7_Sink_initVar;
/** @} globals */


/***************************************
*            API Constants
***************************************/

/* CSD Config register */
#define IDAC7_Sink_CSD_CONFIG_ENABLE_POS    ((uint32)31u)
#define IDAC7_Sink_CSD_CONFIG_SENSE_EN_POS  ((uint32)12u)
#define IDAC7_Sink_CSD_CONFIG_LOW_VDDA_POS  ((uint32)3u)
#define IDAC7_Sink_CSD_CONFIG_SENSE_EN      ((uint32)(0x1u) <<  IDAC7_Sink_CSD_CONFIG_SENSE_EN_POS)
#define IDAC7_Sink_CSD_CONFIG_ENABLE        ((uint32)(0x1u) <<  IDAC7_Sink_CSD_CONFIG_ENABLE_POS)
#define IDAC7_Sink_CSD_CONFIG_LOW_VDDA      ((uint32)(0x1u) <<  IDAC7_Sink_CSD_CONFIG_LOW_VDDA_POS)

/* IDAC register */
#define IDAC7_Sink_CURRENT_VALUE_POS        ((uint32)0u)
#define IDAC7_Sink_POLY_DYN_POS             ((uint32)7u)
#define IDAC7_Sink_POLARITY_POS             ((uint32)8u)
#define IDAC7_Sink_LEG1_MODE_POS            ((uint32)16u)
#define IDAC7_Sink_LEG2_MODE_POS            ((uint32)18u)
#define IDAC7_Sink_DSI_CTRL_EN_POS          ((uint32)21u)
#define IDAC7_Sink_RANGE_POS                ((uint32)22u)
#define IDAC7_Sink_LEG1_EN_POS              ((uint32)24u)
#define IDAC7_Sink_LEG2_EN_POS              ((uint32)25u)

#define IDAC7_Sink_CURRENT_VALUE_MASK       ((uint32)(0x7Fu)    <<  IDAC7_Sink_CURRENT_VALUE_POS)
#define IDAC7_Sink_POLARITY_MASK            ((uint32)(0x3u)     <<  IDAC7_Sink_POLARITY_POS)
#define IDAC7_Sink_POLY_DYN                 ((uint32)(0x1u)     <<  IDAC7_Sink_POLY_DYN_POS)
#define IDAC7_Sink_LEG1_MODE_MASK           ((uint32)(0x3u)     <<  IDAC7_Sink_LEG1_MODE_POS)
#define IDAC7_Sink_LEG2_MODE_MASK           ((uint32)(0x3u)     <<  IDAC7_Sink_LEG2_MODE_POS)
#define IDAC7_Sink_DSI_CTRL_EN              ((uint32)(0x1u)     <<  IDAC7_Sink_DSI_CTRL_EN_POS)
#define IDAC7_Sink_RANGE_MASK               ((uint32)(0x3u)     <<  IDAC7_Sink_RANGE_POS)
#define IDAC7_Sink_LEG1_EN                  ((uint32)(0x1u)     <<  IDAC7_Sink_LEG1_EN_POS)
#define IDAC7_Sink_LEG2_EN                  ((uint32)(0x1u)     <<  IDAC7_Sink_LEG2_EN_POS)

/* Creator-global defines */
#define IDAC7_Sink_2000_MV                  (2000u)

#ifdef CYDEV_VDDA_MV
    #define IDAC7_Sink_CYDEV_VDDA_MV        (CYDEV_VDDA_MV)
#else
    #ifdef CYDEV_VDD_MV
        #define IDAC7_Sink_CYDEV_VDDA_MV    (CYDEV_VDD_MV)
    #endif
#endif

#if defined(CYIPBLOCK_m0s8csdv2_VERSION)
    #define IDAC7_Sink_M0S8CSDV2_BLOCK_VER  (CYIPBLOCK_m0s8csdv2_VERSION)
#else
    #define IDAC7_Sink_M0S8CSDV2_BLOCK_VER  (0u)
#endif /* (CYIPBLOCK_m0s8csdv2_VERSION) */

#define IDAC7_Sink_S8CSDV2_VER_2            (2u)

#if ((IDAC7_Sink_2000_MV > IDAC7_Sink_CYDEV_VDDA_MV) &&\
     (IDAC7_Sink_S8CSDV2_VER_2 == IDAC7_Sink_M0S8CSDV2_BLOCK_VER))
     #define IDAC7_Sink_IDAC_EN_CONFIG      (IDAC7_Sink_CSD_CONFIG_ENABLE |\
                                                   IDAC7_Sink_CSD_CONFIG_SENSE_EN |\
                                                   IDAC7_Sink_CSD_CONFIG_LOW_VDDA)
#else
     #define IDAC7_Sink_IDAC_EN_CONFIG      (IDAC7_Sink_CSD_CONFIG_ENABLE |\
                                                   IDAC7_Sink_CSD_CONFIG_SENSE_EN)
#endif /* (IDAC7_Sink_2000_MV > IDAC7_Sink_CYDEV_VDDA_MV) */

/***************************************
*        Registers
***************************************/

#define IDAC7_Sink_CSD_CONTROL_REG    (*(reg32 *)IDAC7_Sink_cy_psoc4_idac__CONFIG)
#define IDAC7_Sink_CSD_CONTROL_PTR    ( (reg32 *)IDAC7_Sink_cy_psoc4_idac__CONFIG)

#define IDAC7_Sink_IDAC_CONTROL_REG   (*(reg32 *)IDAC7_Sink_cy_psoc4_idac__IDAC)
#define IDAC7_Sink_IDAC_CONTROL_PTR   ( (reg32 *)IDAC7_Sink_cy_psoc4_idac__IDAC)

#endif /* CY_IDAC7_IDAC7_Sink_H */


/* [] END OF FILE */
