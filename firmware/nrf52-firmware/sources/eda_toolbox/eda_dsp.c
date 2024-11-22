/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: EDA DSP
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Provide functions to calculate various outputs from 
 * raw voltage and input signals
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

/*
 * Included files
 */

/* Standard C library includes */

#include <complex.h>

/* SDK includes */

#include "nrf52840.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "arm_const_structs.h"
#include "nrf_log.h"

/* Project includes */

#include "eda_cfg.h"
#include "eda_dsp.h"

/*
 * Local constants
 */

#define FPU_EXCEPTION_MASK               0x0000009F     /**< FPU exception mask used to clear exceptions in FPSCR register. */
#define FPU_FPSCR_REG_STACK_OFF          0x40           /**< Offset of FPSCR register stacked during interrupt handling in FPU part stack. */

static const uint16_t frequency_list[EDA_FREQUENCY_NUM] = EDA_FREQUENCY_LIST;
static float32_t v_tmp[EDA_FFT_BUFFER_SIZE];
static float32_t v_cfft[EDA_FFT_BUFFER_SIZE];

static float32_t i_tmp[EDA_FFT_BUFFER_SIZE];
static float32_t i_cfft[EDA_FFT_BUFFER_SIZE];

/*
 * Local macros
 */

/*
 * Public variables
 */

/*
 * Local types
 */

/*
 * Local variables
 */

static arm_rfft_fast_instance_f32 m_arm_rfft_fast_instance_f32;

static float32_t v_buffer[EDA_FFT_BUFFER_SIZE] = {0};
static float32_t i_buffer[EDA_FFT_BUFFER_SIZE] = {0};

static uint16_t i_buffer_index;

/*
 * Local functions
 */

static void simulated_current(float32_t * i_buffer);

/****************************************************************
 * IMPLEMENTATION
 ****************************************************************/

/*
 * Public functions
 */


/**
 * @brief Initialize FFT instance
 */
void EDA_DSP_Init(void)
{

    /* Enable FPU interrupt to clear flag preventing device from going to sleep */
    NVIC_SetPriority(FPU_IRQn, APP_IRQ_PRIORITY_LOWEST);
    NVIC_ClearPendingIRQ(FPU_IRQn);
    NVIC_EnableIRQ(FPU_IRQn);

    arm_rfft_fast_init_f32(&m_arm_rfft_fast_instance_f32, FFT_CPU_SIZE);
    i_buffer_index = 0;
}


/**
 * @brief Try to clear any remaining FPU interrupt drawing current
 */
void EDA_DSP_Deinit(void)
{
    NVIC_DisableIRQ(FPU_IRQn);
    NVIC_ClearPendingIRQ(FPU_IRQn);
    FPU_IRQHandler();
}


/**
 * @brief Compute impedance at specific frequencies from voltage and current raw data
 */
int EDA_DSP_GetImpedance(int16_t * raw_buffer, Impedance * out_array)
{
    //uint32_t ticks_from = app_timer_cnt_get();

    uint16_t n;

    /* Shift buffers */
    memmove(&v_buffer[0], &v_buffer[EDA_ADC_BUFFER_SIZE], (EDA_FFT_BUFFER_SIZE - EDA_ADC_BUFFER_SIZE) * sizeof(float32_t));
    memmove(&i_buffer[0], &i_buffer[EDA_ADC_BUFFER_SIZE], (EDA_FFT_BUFFER_SIZE - EDA_ADC_BUFFER_SIZE) * sizeof(float32_t));

    /* Extract voltage and current values from raw data buffer */
    for (n = 0; n < EDA_ADC_BUFFER_SIZE; n++)
    {
        v_buffer[(EDA_FFT_BUFFER_SIZE - EDA_ADC_BUFFER_SIZE) + n] = EDA_VOLTAGE_SCALE * (float32_t)raw_buffer[2*n];
        i_buffer[(EDA_FFT_BUFFER_SIZE - EDA_ADC_BUFFER_SIZE) + n] = EDA_CURRENT_SCALE * (float32_t)raw_buffer[(2*n)+1];
    }

    /* Replace current values by theoretical values, might reduce noise */
    if (USE_WAVEFORM == 1)
    {
        simulated_current(&i_buffer[EDA_FFT_BUFFER_SIZE - EDA_ADC_BUFFER_SIZE]);
    }

    /* Copy to a temporary buffer which is modified by fft function */
    memcpy(v_tmp, v_buffer, EDA_FFT_BUFFER_SIZE * sizeof(float32_t));
    memcpy(i_tmp, i_buffer, EDA_FFT_BUFFER_SIZE * sizeof(float32_t));

    /* Compute voltage FFT on temporary buffer because IT IS MODIFIED BY FFT FUNCTION */
    arm_rfft_fast_f32(&m_arm_rfft_fast_instance_f32, v_tmp, v_cfft, 0);

    /* Compute current FFT (also modified, we know that now) */
    arm_rfft_fast_f32(&m_arm_rfft_fast_instance_f32, i_tmp, i_cfft, 0);

    /* Export impedance real and imaginary parts*/
    int index;
    for (n = 0; n < EDA_FREQUENCY_NUM; n ++)
    {
        index = (2*frequency_list[n]/FFT_BIN_RATIO);
        float complex v = v_cfft[index] + v_cfft[index+1] * _Complex_I;
        float complex i = i_cfft[index] + i_cfft[index+1] * _Complex_I;
        float complex y = v / i;
        out_array[n].real = crealf(y);
        out_array[n].imag = cimag(y);
        if (isnan(out_array[n].real) || isnan(out_array[n].imag)) {
            NRF_LOG_WARNING("NaN value for freq. %u (v:%f, i:%f)", frequency_list[n], v, i);
            return -1;
        }
    }

    //uint32_t ticks_to = app_timer_cnt_get();
    //uint32_t delta = app_timer_cnt_diff_compute(ticks_to, ticks_from);
    //NRF_LOG_DEBUG("%lu", delta);
    return 0;
}


/*
 * Local functions
 */

static void simulated_current(float32_t * i_buffer)
{
    uint16_t n;

    for (n = 0; n < EDA_ADC_BUFFER_SIZE; n++)
    {
        i_buffer[n] = EDA_CURRENT_RESOLUTION * (float32_t)i_waveform[(i_buffer_index * EDA_ADC_BUFFER_SIZE) + n];
    }
    i_buffer_index ++;
    if (i_buffer_index >= EDA_ADC_BUFFER_NUM)
    {
        i_buffer_index = 0;
    }
}

void FPU_IRQHandler(void)
{
    // Prepare pointer to stack address with pushed FPSCR register.
    uint32_t * fpscr = (uint32_t * )(FPU->FPCAR + FPU_FPSCR_REG_STACK_OFF);
    // Execute FPU instruction to activate lazy stacking.
    (void)__get_FPSCR();
    // Clear flags in stacked FPSCR register.
    *fpscr = *fpscr & ~(FPU_EXCEPTION_MASK);
}

/* END OF FILE */