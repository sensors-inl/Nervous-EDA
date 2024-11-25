/*******************************************************************************
* File Name: Vout.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Vout_H) /* Pins Vout_H */
#define CY_PINS_Vout_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Vout_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} Vout_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   Vout_Read(void);
void    Vout_Write(uint8 value);
uint8   Vout_ReadDataReg(void);
#if defined(Vout__PC) || (CY_PSOC4_4200L) 
    void    Vout_SetDriveMode(uint8 mode);
#endif
void    Vout_SetInterruptMode(uint16 position, uint16 mode);
uint8   Vout_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void Vout_Sleep(void); 
void Vout_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(Vout__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define Vout_DRIVE_MODE_BITS        (3)
    #define Vout_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Vout_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the Vout_SetDriveMode() function.
         *  @{
         */
        #define Vout_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define Vout_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define Vout_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define Vout_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define Vout_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define Vout_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define Vout_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define Vout_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define Vout_MASK               Vout__MASK
#define Vout_SHIFT              Vout__SHIFT
#define Vout_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Vout_SetInterruptMode() function.
     *  @{
     */
        #define Vout_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define Vout_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define Vout_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define Vout_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(Vout__SIO)
    #define Vout_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(Vout__PC) && (CY_PSOC4_4200L)
    #define Vout_USBIO_ENABLE               ((uint32)0x80000000u)
    #define Vout_USBIO_DISABLE              ((uint32)(~Vout_USBIO_ENABLE))
    #define Vout_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define Vout_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define Vout_USBIO_ENTER_SLEEP          ((uint32)((1u << Vout_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << Vout_USBIO_SUSPEND_DEL_SHIFT)))
    #define Vout_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << Vout_USBIO_SUSPEND_SHIFT)))
    #define Vout_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << Vout_USBIO_SUSPEND_DEL_SHIFT)))
    #define Vout_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(Vout__PC)
    /* Port Configuration */
    #define Vout_PC                 (* (reg32 *) Vout__PC)
#endif
/* Pin State */
#define Vout_PS                     (* (reg32 *) Vout__PS)
/* Data Register */
#define Vout_DR                     (* (reg32 *) Vout__DR)
/* Input Buffer Disable Override */
#define Vout_INP_DIS                (* (reg32 *) Vout__PC2)

/* Interrupt configuration Registers */
#define Vout_INTCFG                 (* (reg32 *) Vout__INTCFG)
#define Vout_INTSTAT                (* (reg32 *) Vout__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define Vout_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(Vout__SIO)
    #define Vout_SIO_REG            (* (reg32 *) Vout__SIO)
#endif /* (Vout__SIO_CFG) */

/* USBIO registers */
#if !defined(Vout__PC) && (CY_PSOC4_4200L)
    #define Vout_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define Vout_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define Vout_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define Vout_DRIVE_MODE_SHIFT       (0x00u)
#define Vout_DRIVE_MODE_MASK        (0x07u << Vout_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins Vout_H */


/* [] END OF FILE */
