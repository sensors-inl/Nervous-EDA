#include "project.h"
#include "idac_array.h"

CY_ISR_PROTO(isr_ext_clk);
volatile uint16_t array_index;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    array_index = 0;
    
    IDAC7_Source_Start();
    IDAC7_Sink_Start();
    Opamp_Vref_Start();
    Opamp_TIA_Start();
    Opamp_Vout_Start();
    Int_Ext_Clk_StartEx(isr_ext_clk);
    
    for(;;)
    {
        /* Place your application code here. */
        CySysPmSleep();
    }
}

CY_ISR(isr_ext_clk)
{
    CyDelayUs(100);
    IDAC7_Source_SetValue(YPOS_Array[array_index]);
    IDAC7_Sink_SetValue(YNEG_Array[array_index]);
    
    array_index ++;
    if (array_index >= IDAC_ARRAY_LENGTH)
    {
        array_index = 0;
    }
    
    Pin_Ext_Clk_ClearInterrupt();
}

/* [] END OF FILE */
