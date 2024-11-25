/*******************************************************************************
* File Name: Vout.h  
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

#if !defined(CY_PINS_Vout_ALIASES_H) /* Pins Vout_ALIASES_H */
#define CY_PINS_Vout_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Vout_0			(Vout__0__PC)
#define Vout_0_PS		(Vout__0__PS)
#define Vout_0_PC		(Vout__0__PC)
#define Vout_0_DR		(Vout__0__DR)
#define Vout_0_SHIFT	(Vout__0__SHIFT)
#define Vout_0_INTR	((uint16)((uint16)0x0003u << (Vout__0__SHIFT*2u)))

#define Vout_INTR_ALL	 ((uint16)(Vout_0_INTR))


#endif /* End Pins Vout_ALIASES_H */


/* [] END OF FILE */
