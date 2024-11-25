/***************************************************************************//**
* \file IDAC7_Source.c
* \version 1.10
*
* \brief
*  This file provides the source code of APIs for the IDAC7 component.
*
*******************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "IDAC7_Source.h"

uint32 IDAC7_Source_initVar = 0u;


/*******************************************************************************
* Function Name: IDAC7_Source_Init
****************************************************************************//**
*
*  Initializes all initial parameters and operating modes.
*
*******************************************************************************/
void IDAC7_Source_Init(void)
{
    uint8 interruptState;

    /* Set IDAC default values */
    interruptState = CyEnterCriticalSection();

    IDAC7_Source_IDAC_CONTROL_REG &= (~IDAC7_Source_POLY_DYN  &
                                          ~IDAC7_Source_LEG1_MODE_MASK &
                                          ~IDAC7_Source_LEG2_MODE_MASK &
                                          ~IDAC7_Source_DSI_CTRL_EN);

    CyExitCriticalSection(interruptState);

    /* Set initial configuration */
    IDAC7_Source_SetValue(IDAC7_Source_CURRENT_VALUE);
    IDAC7_Source_SetPolarity(IDAC7_Source_CURRENT_POLARITY);
    IDAC7_Source_SetRange(IDAC7_Source_CURRENT_RANGE);
}


/*******************************************************************************
* Function Name: IDAC7_Source_Enable
****************************************************************************//**
*
*  Enables the IDAC for operation.
*
*******************************************************************************/
void IDAC7_Source_Enable(void)
{
    uint8 interruptState;

    interruptState = CyEnterCriticalSection();

    /* Enable CSD */
    IDAC7_Source_CSD_CONTROL_REG |= IDAC7_Source_IDAC_EN_CONFIG;

    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: IDAC7_Source_Start
****************************************************************************//**
*
*  Initializes all the parameters required to setup the component as defined
*  in the customizer.
*
*  This component does not stop the CSD IP block. One possible way to turn off 
*  the entire CSD block is to use a specific define (IDAC7_Source_CSD_CONFIG_ENABLE) 
*  for the m0s8csdv2 IP block control register (IDAC7_Source_CSD_CONTROL_REG): 
*  IDAC7_Source_CSD_CONTROL_REG &= ~IDAC7_Source_CSD_CONFIG_ENABLE
*
* \globalvars
*  \ref IDAC7_Source_initVar - this variable is used to indicate the initial
*  configuration of this component. The variable is initialized to zero and
*  set to 1 the first time IDAC7_Source_Start() is called. This allows
*  the component initialization without re-initialization in all subsequent
*  calls to the IDAC7_Source_Start() routine.
*
*******************************************************************************/
void IDAC7_Source_Start(void)
{
    if(0u == IDAC7_Source_initVar)
    {
        IDAC7_Source_Init();
        IDAC7_Source_initVar = 1u;
    }

    IDAC7_Source_Enable();
}


/*******************************************************************************
* Function Name: IDAC7_Source_Stop
****************************************************************************//**
*
*  The Stop is not required.
*
*  This component does not stop the CSD IP block. One possible way to turn off 
*  the entire CSD block is to use a specific define (IDAC7_Source_CSD_CONFIG_ENABLE) 
*  for the m0s8csdv2 IP block control register (IDAC7_Source_CSD_CONTROL_REG): 
*  IDAC7_Source_CSD_CONTROL_REG &= ~IDAC7_Source_CSD_CONFIG_ENABLE
*
*******************************************************************************/
void IDAC7_Source_Stop(void)
{
    /* Do nothing */
}


/*******************************************************************************
* Function Name: IDAC7_Source_SetValue
****************************************************************************//**
*
*  Sets the IDAC current to the new value.
*
*  \param uint32 current: The current value
*  * Valid range    : [0 - 127]
*
*******************************************************************************/
void IDAC7_Source_SetValue(uint32 current)
{
    uint8 interruptState;
    uint32 newRegisterValue;

    interruptState = CyEnterCriticalSection();

    newRegisterValue = IDAC7_Source_IDAC_CONTROL_REG & ~IDAC7_Source_CURRENT_VALUE_MASK;

    newRegisterValue |= ((current << IDAC7_Source_CURRENT_VALUE_POS) & IDAC7_Source_CURRENT_VALUE_MASK);

    IDAC7_Source_IDAC_CONTROL_REG = newRegisterValue;

    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: IDAC7_Source_SetPolarity
****************************************************************************//**
*
*  Sets polarity to either sink or source.
*
*  \param uint32 polarity: Current polarity
*  * IDAC7_Source_POL_SOURCE   : Source polarity
*  * IDAC7_Source_POL_SINK     : Sink polarity
*
*******************************************************************************/
void IDAC7_Source_SetPolarity(uint32 polarity)
{
    uint8 interruptState;
    uint32 newRegisterValue;

    interruptState = CyEnterCriticalSection();

    newRegisterValue = IDAC7_Source_IDAC_CONTROL_REG & ~IDAC7_Source_POLARITY_MASK;

    newRegisterValue |= ((polarity << IDAC7_Source_POLARITY_POS) & IDAC7_Source_POLARITY_MASK);

    IDAC7_Source_IDAC_CONTROL_REG = newRegisterValue;

    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: IDAC7_Source_SetRange
****************************************************************************//**
*
* Sets the IDAC range to one of the six ranges.
*
*  \param uint32 range: Current range
*  * IDAC7_Source_RNG_4_76UA   : 37.5 nA/bit current range
*  * IDAC7_Source_RNG_9_52UA   : 75 nA/bit current range
*  * IDAC7_Source_RNG_38_1UA   : 300 nA/bit current range
*  * IDAC7_Source_RNG_76_2UA   : 600 nA/bit current range
*  * IDAC7_Source_RNG_152_4UA  : 1.2 uA/bit current range
*  * IDAC7_Source_RNG_304_8UA  : 2.4 uA/bit current range
*  * IDAC7_Source_RNG_609_6UA  : 4.8 uA/bit current range
*
*******************************************************************************/
void IDAC7_Source_SetRange(uint32 range)
{
    uint8 interruptState;
    uint32 newRegisterValue;

    interruptState = CyEnterCriticalSection();

    newRegisterValue = IDAC7_Source_IDAC_CONTROL_REG & (~IDAC7_Source_RANGE_MASK    &
                                                            ~IDAC7_Source_LEG1_EN       &
                                                            ~IDAC7_Source_LEG2_EN);

    newRegisterValue |= (range << IDAC7_Source_RANGE_POS);

    IDAC7_Source_IDAC_CONTROL_REG = newRegisterValue;

    CyExitCriticalSection(interruptState);
}


/* [] END OF FILE */
