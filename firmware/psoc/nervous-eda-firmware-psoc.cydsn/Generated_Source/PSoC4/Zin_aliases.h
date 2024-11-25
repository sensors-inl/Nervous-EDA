/*******************************************************************************
* File Name: Zin.h  
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

#if !defined(CY_PINS_Zin_ALIASES_H) /* Pins Zin_ALIASES_H */
#define CY_PINS_Zin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Zin_0			(Zin__0__PC)
#define Zin_0_PS		(Zin__0__PS)
#define Zin_0_PC		(Zin__0__PC)
#define Zin_0_DR		(Zin__0__DR)
#define Zin_0_SHIFT	(Zin__0__SHIFT)
#define Zin_0_INTR	((uint16)((uint16)0x0003u << (Zin__0__SHIFT*2u)))

#define Zin_INTR_ALL	 ((uint16)(Zin_0_INTR))


#endif /* End Pins Zin_ALIASES_H */


/* [] END OF FILE */
