/*******************************************************************************
* File Name: Opamp_Vout.h
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


#if !defined(CY_OPAMP_Opamp_Vout_H)
#define CY_OPAMP_Opamp_Vout_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*       Type Definitions
***************************************/

/* Structure to save state before go to sleep */
typedef struct
{
    uint8  enableState;
} Opamp_Vout_BACKUP_STRUCT;


/**************************************
*        Function Prototypes
**************************************/
void Opamp_Vout_Init(void);
void Opamp_Vout_Enable(void);
void Opamp_Vout_Start(void);
void Opamp_Vout_Stop(void);
void Opamp_Vout_SetPower(uint32 power);
void Opamp_Vout_PumpControl(uint32 onOff);
void Opamp_Vout_Sleep(void);
void Opamp_Vout_Wakeup(void);
void Opamp_Vout_SaveConfig(void);
void Opamp_Vout_RestoreConfig(void);


/**************************************
*           API Constants
**************************************/

/* Parameters for SetPower() function */
#define Opamp_Vout_LOW_POWER      (1u)
#define Opamp_Vout_MED_POWER      (2u)
#define Opamp_Vout_HIGH_POWER     (3u)


/* Parameters for PumpControl() function */
#define Opamp_Vout_PUMP_ON        (1u)
#define Opamp_Vout_PUMP_OFF       (0u)


/***************************************
*   Initial Parameter Constants
****************************************/

#define Opamp_Vout_OUTPUT_CURRENT         (1u)
#define Opamp_Vout_POWER                  (1u)
#define Opamp_Vout_MODE                   (1u)
#define Opamp_Vout_OA_COMP_TRIM_VALUE     (3u)
#define Opamp_Vout_DEEPSLEEP_SUPPORT      (0u)


/***************************************
*    Variables with External Linkage
***************************************/

extern uint8  Opamp_Vout_initVar;


/**************************************
*             Registers
**************************************/

#ifdef CYIPBLOCK_m0s8pass4b_VERSION
    #define Opamp_Vout_CTB_CTRL_REG       (*(reg32 *) Opamp_Vout_cy_psoc4_abuf__CTB_CTB_CTRL)
    #define Opamp_Vout_CTB_CTRL_PTR       ( (reg32 *) Opamp_Vout_cy_psoc4_abuf__CTB_CTB_CTRL)
#else
    #define Opamp_Vout_CTB_CTRL_REG       (*(reg32 *) Opamp_Vout_cy_psoc4_abuf__CTBM_CTB_CTRL)
    #define Opamp_Vout_CTB_CTRL_PTR       ( (reg32 *) Opamp_Vout_cy_psoc4_abuf__CTBM_CTB_CTRL)
#endif /* CYIPBLOCK_m0s8pass4b_VERSION */

#define Opamp_Vout_OA_RES_CTRL_REG    (*(reg32 *) Opamp_Vout_cy_psoc4_abuf__OA_RES_CTRL)
#define Opamp_Vout_OA_RES_CTRL_PTR    ( (reg32 *) Opamp_Vout_cy_psoc4_abuf__OA_RES_CTRL)
#define Opamp_Vout_OA_COMP_TRIM_REG   (*(reg32 *) Opamp_Vout_cy_psoc4_abuf__OA_COMP_TRIM)
#define Opamp_Vout_OA_COMP_TRIM_PTR   ( (reg32 *) Opamp_Vout_cy_psoc4_abuf__OA_COMP_TRIM)


/***************************************
*        Registers Constants
***************************************/

/* Opamp_Vout_CTB_CTRL_REG */
#define Opamp_Vout_CTB_CTRL_DEEPSLEEP_ON_SHIFT    (30u)   /* [30] Selects behavior CTB IP in the DeepSleep power mode */
#define Opamp_Vout_CTB_CTRL_ENABLED_SHIFT         (31u)   /* [31] Enable of the CTB IP */


#define Opamp_Vout_CTB_CTRL_DEEPSLEEP_ON          ((uint32) 0x01u << Opamp_Vout_CTB_CTRL_DEEPSLEEP_ON_SHIFT)
#define Opamp_Vout_CTB_CTRL_ENABLED               ((uint32) 0x01u << Opamp_Vout_CTB_CTRL_ENABLED_SHIFT)


/* Opamp_Vout_OA_RES_CTRL_REG */
#define Opamp_Vout_OA_PWR_MODE_SHIFT          (0u)    /* [1:0]    Power level */
#define Opamp_Vout_OA_DRIVE_STR_SEL_SHIFT     (2u)    /* [2]      Opamp output strenght select: 0 - 1x, 1 - 10x */
#define Opamp_Vout_OA_COMP_EN_SHIFT           (4u)    /* [4]      CTB IP mode: 0 - Opamp, 1 - Comparator  */
#define Opamp_Vout_OA_PUMP_EN_SHIFT           (11u)   /* [11]     Pump enable */


#define Opamp_Vout_OA_PWR_MODE                ((uint32) 0x02u << Opamp_Vout_OA_PWR_MODE_SHIFT)
#define Opamp_Vout_OA_PWR_MODE_MASK           ((uint32) 0x03u << Opamp_Vout_OA_PWR_MODE_SHIFT)
#define Opamp_Vout_OA_DRIVE_STR_SEL_1X        ((uint32) 0x00u << Opamp_Vout_OA_DRIVE_STR_SEL_SHIFT)
#define Opamp_Vout_OA_DRIVE_STR_SEL_10X       ((uint32) 0x01u << Opamp_Vout_OA_DRIVE_STR_SEL_SHIFT)
#define Opamp_Vout_OA_DRIVE_STR_SEL_MASK      ((uint32) 0x01u << Opamp_Vout_OA_DRIVE_STR_SEL_SHIFT)
#define Opamp_Vout_OA_COMP_EN                 ((uint32) 0x00u << Opamp_Vout_OA_COMP_EN_SHIFT)
#define Opamp_Vout_OA_PUMP_EN                 ((uint32) 0x01u << Opamp_Vout_OA_PUMP_EN_SHIFT)


/***************************************
*       Init Macros Definitions
***************************************/

#define Opamp_Vout_GET_DEEPSLEEP_ON(deepSleep)    ((0u != (deepSleep)) ? (Opamp_Vout_CTB_CTRL_DEEPSLEEP_ON) : (0u))
#define Opamp_Vout_GET_OA_DRIVE_STR(current)      ((0u != (current)) ? (Opamp_Vout_OA_DRIVE_STR_SEL_10X) : \
                                                                             (Opamp_Vout_OA_DRIVE_STR_SEL_1X))
#define Opamp_Vout_GET_OA_PWR_MODE(mode)          ((mode) & Opamp_Vout_OA_PWR_MODE_MASK)
#define Opamp_Vout_CHECK_PWR_MODE_OFF             (0u != (Opamp_Vout_OA_RES_CTRL_REG & \
                                                                Opamp_Vout_OA_PWR_MODE_MASK))

/* Returns true if component available in Deep Sleep power mode*/ 
#define Opamp_Vout_CHECK_DEEPSLEEP_SUPPORT        (0u != Opamp_Vout_DEEPSLEEP_SUPPORT) 

#define Opamp_Vout_DEFAULT_CTB_CTRL (Opamp_Vout_GET_DEEPSLEEP_ON(Opamp_Vout_DEEPSLEEP_SUPPORT) | \
                                           Opamp_Vout_CTB_CTRL_ENABLED)

#define Opamp_Vout_DEFAULT_OA_RES_CTRL (Opamp_Vout_OA_COMP_EN | \
                                              Opamp_Vout_GET_OA_DRIVE_STR(Opamp_Vout_OUTPUT_CURRENT))

#define Opamp_Vout_DEFAULT_OA_COMP_TRIM_REG (Opamp_Vout_OA_COMP_TRIM_VALUE)


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define Opamp_Vout_LOWPOWER                   (Opamp_Vout_LOW_POWER)
#define Opamp_Vout_MEDPOWER                   (Opamp_Vout_MED_POWER)
#define Opamp_Vout_HIGHPOWER                  (Opamp_Vout_HIGH_POWER)

/* PUMP ON/OFF defines */
#define Opamp_Vout_PUMPON                     (Opamp_Vout_PUMP_ON)
#define Opamp_Vout_PUMPOFF                    (Opamp_Vout_PUMP_OFF)

#define Opamp_Vout_OA_CTRL                    (Opamp_Vout_CTB_CTRL_REG)
#define Opamp_Vout_OA_RES_CTRL                (Opamp_Vout_OA_RES_CTRL_REG)

/* Bit Field  OA_CTRL */
#define Opamp_Vout_OA_CTB_EN_SHIFT            (Opamp_Vout_CTB_CTRL_ENABLED_SHIFT)
#define Opamp_Vout_OA_PUMP_CTRL_SHIFT         (Opamp_Vout_OA_PUMP_EN_SHIFT)
#define Opamp_Vout_OA_PUMP_EN_MASK            (0x800u)
#define Opamp_Vout_PUMP_PROTECT_MASK          (1u)


#endif    /* CY_OPAMP_Opamp_Vout_H */


/* [] END OF FILE */
