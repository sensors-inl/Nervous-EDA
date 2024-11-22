/*** This file was generated, do not modify ***/

#define EDA_FREQUENCY_LIST          {12, 28, 32, 36, 44, 68, 84, 108, 136, 196, 256, 324, 400, 484, 576, 724};  /**< values of frequencies in the waveform, also equal to FFT bin index */
#define EDA_FREQUENCY_NUM           16 /**< number of frequencies contained in the waveform */
#define EDA_FREQUENCY_BIN_RATIO     2 /**< divide frequency by this number to get index in FFT */
#define EDA_SAMPLING_RATE           4096 /**< SAADC sampling rate, identical to clock generated for AFE */
#define EDA_FFT_BUFFER_SIZE         2048 /**< should be equal to EDA_ADC_BUFFER_SIZE * EDA_ADC_BUFFER_NUM */
#define EDA_ADC_BUFFER_SIZE         512 /**< number of samples for each v and i */
#define EDA_ADC_BUFFER_NUM          4 /**< number of SAADC buffers to fill one FFT buffer */
#define IDAC_ARRAY_LENGTH   4096
extern const unsigned char YPOS_Array[IDAC_ARRAY_LENGTH];
extern const unsigned char YNEG_Array[IDAC_ARRAY_LENGTH];
