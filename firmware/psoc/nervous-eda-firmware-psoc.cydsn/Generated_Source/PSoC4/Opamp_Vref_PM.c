/*******************************************************************************
* File Name: Opamp_Vref_PM.c
* Version 1.20
*
* Description:
*  This file provides the power management source code to the API for the
*  Opamp (Analog Buffer) component.
*
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Opamp_Vref.h"

#if(!Opamp_Vref_CHECK_DEEPSLEEP_SUPPORT)
    static Opamp_Vref_BACKUP_STRUCT Opamp_Vref_backup =
    {
        0u, /* enableState */
    };
#endif /* (Opamp_Vref_CHECK_DEEPSLEEP_SUPPORT) */


/*******************************************************************************
* Function Name: Opamp_Vref_SaveConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Opamp_Vref_SaveConfig(void)
{

}


/*******************************************************************************
* Function Name: Opamp_Vref_RestoreConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Opamp_Vref_RestoreConfig(void)
{
    
}


/*******************************************************************************
* Function Name: Opamp_Vref_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. The Sleep() API 
*  saves the current component state. Call the Sleep() function before calling the 
*  CySysPmDeepSleep() or the CySysPmHibernate() functions. The "Deep sleep operation" 
*  option has an influence on this function implementation.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Opamp_Vref_backup: The structure field 'enableState' is modified
*  depending on the enable state of the block before entering the sleep mode.
*
*******************************************************************************/
void Opamp_Vref_Sleep(void)
{
#if(!Opamp_Vref_CHECK_DEEPSLEEP_SUPPORT)
    if(Opamp_Vref_CHECK_PWR_MODE_OFF)
    {
        Opamp_Vref_backup.enableState = 1u;
        Opamp_Vref_Stop();
    }
    else /* The component is disabled */
    {
        Opamp_Vref_backup.enableState = 0u;
    }
#endif /* (Opamp_Vref_CHECK_DEEPSLEEP_SUPPORT) */
}


/*******************************************************************************
* Function Name: Opamp_Vref_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when Sleep() 
*  is called. If the component has been enabled before the Sleep() function is 
*  called, the Wakeup() function will also re-enable the component.
*  The "Deep sleep operation" option has an influence on this function
*  implementation.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Opamp_Vref_backup: The structure field 'enableState' is used to
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void Opamp_Vref_Wakeup(void)
{
#if(!Opamp_Vref_CHECK_DEEPSLEEP_SUPPORT)
    if(0u != Opamp_Vref_backup.enableState)
    {
        /* Enable Opamp's operation */
        Opamp_Vref_Enable();
    } /* Do nothing if Opamp was disabled before */
#endif /* (Opamp_Vref_CHECK_DEEPSLEEP_SUPPORT) */
}


/* [] END OF FILE */
