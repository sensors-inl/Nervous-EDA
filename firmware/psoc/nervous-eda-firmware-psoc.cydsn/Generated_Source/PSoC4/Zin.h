/*******************************************************************************
* File Name: Zin.h  
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

#if !defined(CY_PINS_Zin_H) /* Pins Zin_H */
#define CY_PINS_Zin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Zin_aliases.h"


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
} Zin_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   Zin_Read(void);
void    Zin_Write(uint8 value);
uint8   Zin_ReadDataReg(void);
#if defined(Zin__PC) || (CY_PSOC4_4200L) 
    void    Zin_SetDriveMode(uint8 mode);
#endif
void    Zin_SetInterruptMode(uint16 position, uint16 mode);
uint8   Zin_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void Zin_Sleep(void); 
void Zin_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(Zin__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define Zin_DRIVE_MODE_BITS        (3)
    #define Zin_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Zin_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the Zin_SetDriveMode() function.
         *  @{
         */
        #define Zin_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define Zin_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define Zin_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define Zin_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define Zin_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define Zin_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define Zin_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define Zin_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define Zin_MASK               Zin__MASK
#define Zin_SHIFT              Zin__SHIFT
#define Zin_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Zin_SetInterruptMode() function.
     *  @{
     */
        #define Zin_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define Zin_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define Zin_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define Zin_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(Zin__SIO)
    #define Zin_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(Zin__PC) && (CY_PSOC4_4200L)
    #define Zin_USBIO_ENABLE               ((uint32)0x80000000u)
    #define Zin_USBIO_DISABLE              ((uint32)(~Zin_USBIO_ENABLE))
    #define Zin_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define Zin_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define Zin_USBIO_ENTER_SLEEP          ((uint32)((1u << Zin_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << Zin_USBIO_SUSPEND_DEL_SHIFT)))
    #define Zin_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << Zin_USBIO_SUSPEND_SHIFT)))
    #define Zin_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << Zin_USBIO_SUSPEND_DEL_SHIFT)))
    #define Zin_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(Zin__PC)
    /* Port Configuration */
    #define Zin_PC                 (* (reg32 *) Zin__PC)
#endif
/* Pin State */
#define Zin_PS                     (* (reg32 *) Zin__PS)
/* Data Register */
#define Zin_DR                     (* (reg32 *) Zin__DR)
/* Input Buffer Disable Override */
#define Zin_INP_DIS                (* (reg32 *) Zin__PC2)

/* Interrupt configuration Registers */
#define Zin_INTCFG                 (* (reg32 *) Zin__INTCFG)
#define Zin_INTSTAT                (* (reg32 *) Zin__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define Zin_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(Zin__SIO)
    #define Zin_SIO_REG            (* (reg32 *) Zin__SIO)
#endif /* (Zin__SIO_CFG) */

/* USBIO registers */
#if !defined(Zin__PC) && (CY_PSOC4_4200L)
    #define Zin_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define Zin_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define Zin_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define Zin_DRIVE_MODE_SHIFT       (0x00u)
#define Zin_DRIVE_MODE_MASK        (0x07u << Zin_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins Zin_H */


/* [] END OF FILE */
