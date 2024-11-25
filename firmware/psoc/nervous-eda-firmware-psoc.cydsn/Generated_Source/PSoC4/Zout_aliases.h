/*******************************************************************************
* File Name: Zout.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Zout_ALIASES_H) /* Pins Zout_ALIASES_H */
#define CY_PINS_Zout_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Zout_0			(Zout__0__PC)
#define Zout_0_PS		(Zout__0__PS)
#define Zout_0_PC		(Zout__0__PC)
#define Zout_0_DR		(Zout__0__DR)
#define Zout_0_SHIFT	(Zout__0__SHIFT)
#define Zout_0_INTR	((uint16)((uint16)0x0003u << (Zout__0__SHIFT*2u)))

#define Zout_INTR_ALL	 ((uint16)(Zout_0_INTR))


#endif /* End Pins Zout_ALIASES_H */


/* [] END OF FILE */
