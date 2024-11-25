/*******************************************************************************
* File Name: Zout.h  
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

#if !defined(CY_PINS_Zout_H) /* Pins Zout_H */
#define CY_PINS_Zout_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Zout_aliases.h"


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
} Zout_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   Zout_Read(void);
void    Zout_Write(uint8 value);
uint8   Zout_ReadDataReg(void);
#if defined(Zout__PC) || (CY_PSOC4_4200L) 
    void    Zout_SetDriveMode(uint8 mode);
#endif
void    Zout_SetInterruptMode(uint16 position, uint16 mode);
uint8   Zout_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void Zout_Sleep(void); 
void Zout_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(Zout__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define Zout_DRIVE_MODE_BITS        (3)
    #define Zout_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Zout_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the Zout_SetDriveMode() function.
         *  @{
         */
        #define Zout_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define Zout_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define Zout_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define Zout_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define Zout_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define Zout_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define Zout_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define Zout_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define Zout_MASK               Zout__MASK
#define Zout_SHIFT              Zout__SHIFT
#define Zout_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Zout_SetInterruptMode() function.
     *  @{
     */
        #define Zout_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define Zout_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define Zout_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define Zout_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(Zout__SIO)
    #define Zout_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(Zout__PC) && (CY_PSOC4_4200L)
    #define Zout_USBIO_ENABLE               ((uint32)0x80000000u)
    #define Zout_USBIO_DISABLE              ((uint32)(~Zout_USBIO_ENABLE))
    #define Zout_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define Zout_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define Zout_USBIO_ENTER_SLEEP          ((uint32)((1u << Zout_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << Zout_USBIO_SUSPEND_DEL_SHIFT)))
    #define Zout_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << Zout_USBIO_SUSPEND_SHIFT)))
    #define Zout_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << Zout_USBIO_SUSPEND_DEL_SHIFT)))
    #define Zout_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(Zout__PC)
    /* Port Configuration */
    #define Zout_PC                 (* (reg32 *) Zout__PC)
#endif
/* Pin State */
#define Zout_PS                     (* (reg32 *) Zout__PS)
/* Data Register */
#define Zout_DR                     (* (reg32 *) Zout__DR)
/* Input Buffer Disable Override */
#define Zout_INP_DIS                (* (reg32 *) Zout__PC2)

/* Interrupt configuration Registers */
#define Zout_INTCFG                 (* (reg32 *) Zout__INTCFG)
#define Zout_INTSTAT                (* (reg32 *) Zout__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define Zout_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(Zout__SIO)
    #define Zout_SIO_REG            (* (reg32 *) Zout__SIO)
#endif /* (Zout__SIO_CFG) */

/* USBIO registers */
#if !defined(Zout__PC) && (CY_PSOC4_4200L)
    #define Zout_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define Zout_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define Zout_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define Zout_DRIVE_MODE_SHIFT       (0x00u)
#define Zout_DRIVE_MODE_MASK        (0x07u << Zout_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins Zout_H */


/* [] END OF FILE */
