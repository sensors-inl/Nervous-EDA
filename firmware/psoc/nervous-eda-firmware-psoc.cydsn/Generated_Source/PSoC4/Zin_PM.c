/*******************************************************************************
* File Name: Zin.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Zin.h"

static Zin_BACKUP_STRUCT  Zin_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: Zin_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet Zin_SUT.c usage_Zin_Sleep_Wakeup
*******************************************************************************/
void Zin_Sleep(void)
{
    #if defined(Zin__PC)
        Zin_backup.pcState = Zin_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            Zin_backup.usbState = Zin_CR1_REG;
            Zin_USB_POWER_REG |= Zin_USBIO_ENTER_SLEEP;
            Zin_CR1_REG &= Zin_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Zin__SIO)
        Zin_backup.sioState = Zin_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        Zin_SIO_REG &= (uint32)(~Zin_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: Zin_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to Zin_Sleep() for an example usage.
*******************************************************************************/
void Zin_Wakeup(void)
{
    #if defined(Zin__PC)
        Zin_PC = Zin_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            Zin_USB_POWER_REG &= Zin_USBIO_EXIT_SLEEP_PH1;
            Zin_CR1_REG = Zin_backup.usbState;
            Zin_USB_POWER_REG &= Zin_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Zin__SIO)
        Zin_SIO_REG = Zin_backup.sioState;
    #endif
}


/* [] END OF FILE */
