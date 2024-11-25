/*******************************************************************************
* File Name: Opamp_Vref.h
* Version 1.20
*
* Description:
*  This file contains the function prototypes and constants used in
*  the Opamp (Analog Buffer) Component.
*
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_OPAMP_Opamp_Vref_H)
#define CY_OPAMP_Opamp_Vref_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*       Type Definitions
***************************************/

/* Structure to save state before go to sleep */
typedef struct
{
    uint8  enableState;
} Opamp_Vref_BACKUP_STRUCT;


/**************************************
*        Function Prototypes
**************************************/
void Opamp_Vref_Init(void);
void Opamp_Vref_Enable(void);
void Opamp_Vref_Start(void);
void Opamp_Vref_Stop(void);
void Opamp_Vref_SetPower(uint32 power);
void Opamp_Vref_PumpControl(uint32 onOff);
void Opamp_Vref_Sleep(void);
void Opamp_Vref_Wakeup(void);
void Opamp_Vref_SaveConfig(void);
void Opamp_Vref_RestoreConfig(void);


/**************************************
*           API Constants
**************************************/

/* Parameters for SetPower() function */
#define Opamp_Vref_LOW_POWER      (1u)
#define Opamp_Vref_MED_POWER      (2u)
#define Opamp_Vref_HIGH_POWER     (3u)


/* Parameters for PumpControl() function */
#define Opamp_Vref_PUMP_ON        (1u)
#define Opamp_Vref_PUMP_OFF       (0u)


/***************************************
*   Initial Parameter Constants
****************************************/

#define Opamp_Vref_OUTPUT_CURRENT         (1u)
#define Opamp_Vref_POWER                  (1u)
#define Opamp_Vref_MODE                   (1u)
#define Opamp_Vref_OA_COMP_TRIM_VALUE     (3u)
#define Opamp_Vref_DEEPSLEEP_SUPPORT      (0u)


/***************************************
*    Variables with External Linkage
***************************************/

extern uint8  Opamp_Vref_initVar;


/**************************************
*             Registers
**************************************/

#ifdef CYIPBLOCK_m0s8pass4b_VERSION
    #define Opamp_Vref_CTB_CTRL_REG       (*(reg32 *) Opamp_Vref_cy_psoc4_abuf__CTB_CTB_CTRL)
    #define Opamp_Vref_CTB_CTRL_PTR       ( (reg32 *) Opamp_Vref_cy_psoc4_abuf__CTB_CTB_CTRL)
#else
    #define Opamp_Vref_CTB_CTRL_REG       (*(reg32 *) Opamp_Vref_cy_psoc4_abuf__CTBM_CTB_CTRL)
    #define Opamp_Vref_CTB_CTRL_PTR       ( (reg32 *) Opamp_Vref_cy_psoc4_abuf__CTBM_CTB_CTRL)
#endif /* CYIPBLOCK_m0s8pass4b_VERSION */

#define Opamp_Vref_OA_RES_CTRL_REG    (*(reg32 *) Opamp_Vref_cy_psoc4_abuf__OA_RES_CTRL)
#define Opamp_Vref_OA_RES_CTRL_PTR    ( (reg32 *) Opamp_Vref_cy_psoc4_abuf__OA_RES_CTRL)
#define Opamp_Vref_OA_COMP_TRIM_REG   (*(reg32 *) Opamp_Vref_cy_psoc4_abuf__OA_COMP_TRIM)
#define Opamp_Vref_OA_COMP_TRIM_PTR   ( (reg32 *) Opamp_Vref_cy_psoc4_abuf__OA_COMP_TRIM)


/***************************************
*        Registers Constants
***************************************/

/* Opamp_Vref_CTB_CTRL_REG */
#define Opamp_Vref_CTB_CTRL_DEEPSLEEP_ON_SHIFT    (30u)   /* [30] Selects behavior CTB IP in the DeepSleep power mode */
#define Opamp_Vref_CTB_CTRL_ENABLED_SHIFT         (31u)   /* [31] Enable of the CTB IP */


#define Opamp_Vref_CTB_CTRL_DEEPSLEEP_ON          ((uint32) 0x01u << Opamp_Vref_CTB_CTRL_DEEPSLEEP_ON_SHIFT)
#define Opamp_Vref_CTB_CTRL_ENABLED               ((uint32) 0x01u << Opamp_Vref_CTB_CTRL_ENABLED_SHIFT)


/* Opamp_Vref_OA_RES_CTRL_REG */
#define Opamp_Vref_OA_PWR_MODE_SHIFT          (0u)    /* [1:0]    Power level */
#define Opamp_Vref_OA_DRIVE_STR_SEL_SHIFT     (2u)    /* [2]      Opamp output strenght select: 0 - 1x, 1 - 10x */
#define Opamp_Vref_OA_COMP_EN_SHIFT           (4u)    /* [4]      CTB IP mode: 0 - Opamp, 1 - Comparator  */
#define Opamp_Vref_OA_PUMP_EN_SHIFT           (11u)   /* [11]     Pump enable */


#define Opamp_Vref_OA_PWR_MODE                ((uint32) 0x02u << Opamp_Vref_OA_PWR_MODE_SHIFT)
#define Opamp_Vref_OA_PWR_MODE_MASK           ((uint32) 0x03u << Opamp_Vref_OA_PWR_MODE_SHIFT)
#define Opamp_Vref_OA_DRIVE_STR_SEL_1X        ((uint32) 0x00u << Opamp_Vref_OA_DRIVE_STR_SEL_SHIFT)
#define Opamp_Vref_OA_DRIVE_STR_SEL_10X       ((uint32) 0x01u << Opamp_Vref_OA_DRIVE_STR_SEL_SHIFT)
#define Opamp_Vref_OA_DRIVE_STR_SEL_MASK      ((uint32) 0x01u << Opamp_Vref_OA_DRIVE_STR_SEL_SHIFT)
#define Opamp_Vref_OA_COMP_EN                 ((uint32) 0x00u << Opamp_Vref_OA_COMP_EN_SHIFT)
#define Opamp_Vref_OA_PUMP_EN                 ((uint32) 0x01u << Opamp_Vref_OA_PUMP_EN_SHIFT)


/***************************************
*       Init Macros Definitions
***************************************/

#define Opamp_Vref_GET_DEEPSLEEP_ON(deepSleep)    ((0u != (deepSleep)) ? (Opamp_Vref_CTB_CTRL_DEEPSLEEP_ON) : (0u))
#define Opamp_Vref_GET_OA_DRIVE_STR(current)      ((0u != (current)) ? (Opamp_Vref_OA_DRIVE_STR_SEL_10X) : \
                                                                             (Opamp_Vref_OA_DRIVE_STR_SEL_1X))
#define Opamp_Vref_GET_OA_PWR_MODE(mode)          ((mode) & Opamp_Vref_OA_PWR_MODE_MASK)
#define Opamp_Vref_CHECK_PWR_MODE_OFF             (0u != (Opamp_Vref_OA_RES_CTRL_REG & \
                                                                Opamp_Vref_OA_PWR_MODE_MASK))

/* Returns true if component available in Deep Sleep power mode*/ 
#define Opamp_Vref_CHECK_DEEPSLEEP_SUPPORT        (0u != Opamp_Vref_DEEPSLEEP_SUPPORT) 

#define Opamp_Vref_DEFAULT_CTB_CTRL (Opamp_Vref_GET_DEEPSLEEP_ON(Opamp_Vref_DEEPSLEEP_SUPPORT) | \
                                           Opamp_Vref_CTB_CTRL_ENABLED)

#define Opamp_Vref_DEFAULT_OA_RES_CTRL (Opamp_Vref_OA_COMP_EN | \
                                              Opamp_Vref_GET_OA_DRIVE_STR(Opamp_Vref_OUTPUT_CURRENT))

#define Opamp_Vref_DEFAULT_OA_COMP_TRIM_REG (Opamp_Vref_OA_COMP_TRIM_VALUE)


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define Opamp_Vref_LOWPOWER                   (Opamp_Vref_LOW_POWER)
#define Opamp_Vref_MEDPOWER                   (Opamp_Vref_MED_POWER)
#define Opamp_Vref_HIGHPOWER                  (Opamp_Vref_HIGH_POWER)

/* PUMP ON/OFF defines */
#define Opamp_Vref_PUMPON                     (Opamp_Vref_PUMP_ON)
#define Opamp_Vref_PUMPOFF                    (Opamp_Vref_PUMP_OFF)

#define Opamp_Vref_OA_CTRL                    (Opamp_Vref_CTB_CTRL_REG)
#define Opamp_Vref_OA_RES_CTRL                (Opamp_Vref_OA_RES_CTRL_REG)

/* Bit Field  OA_CTRL */
#define Opamp_Vref_OA_CTB_EN_SHIFT            (Opamp_Vref_CTB_CTRL_ENABLED_SHIFT)
#define Opamp_Vref_OA_PUMP_CTRL_SHIFT         (Opamp_Vref_OA_PUMP_EN_SHIFT)
#define Opamp_Vref_OA_PUMP_EN_MASK            (0x800u)
#define Opamp_Vref_PUMP_PROTECT_MASK          (1u)


#endif    /* CY_OPAMP_Opamp_Vref_H */


/* [] END OF FILE */
