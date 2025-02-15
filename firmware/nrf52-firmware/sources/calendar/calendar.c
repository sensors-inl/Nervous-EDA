/****************************************************************
 * Project: RENFORCE EDA FIRMWARE
 * Module: CALENDAR
 * Author: Bertrand Massot
 * Mail: bertrand.massot@insa-lyon.fr
 * 
 *---------------------------------------------------------------
 * @brief Real time calendar functions
 *
 *---------------------------------------------------------------
 * Copyright (c) 2023 INL - INSA LYON
 ****************************************************************/

/*
 * Included files
 */

/* Standard C library includes */
#include "stdint.h"
#include "time.h"

/* SDK includes */
#include "nrfx_rtc.h"
#include "nrf_error.h"
#include "app_error.h"

#define NRF_LOG_MODULE_NAME CAL
#define NRF_LOG_INFO_COLOR  3
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#include "nrf_drv_clock.h"

/* Project includes */
#include "calendar.h"

/*
 * Local constants
 */

/*
 * Local macros
 */

/*
 * Public variables
 */

/*
 * Local variables
 */

/*
 * Local functions
 */

/* RTC Hardware Abstraction Layer */

static void rtc_init(void);
static void rtc_deinit(void);
static void rtc_set_time(const uint64_t timestamp, const uint32_t us);
static void rtc_get_time(uint64_t * p_timestamp, uint32_t * p_us);
static void rtc_evt_handler(nrfx_rtc_int_type_t type);
static nrfx_rtc_t * rtc_get_instance(void);

/****************************************************************
 * IMPLEMENTATION
 ****************************************************************/

/*
 * Public functions
 */

/**
 * @brief Initialize the calendar
 */
void CAL_Init(void)
{
    rtc_init();
    rtc_set_time(0ULL, 0UL);
}

/**
 * @brief Release the calendar
 */
void CAL_Deinit(void)
{
    rtc_deinit();
}


/**
 * @brief Update calendar time with Unix timestamp
 * @param[in] timestamp containing a Unix epoch
 */
void CAL_SetTime(const uint64_t timestamp, const uint32_t us)
{
    rtc_set_time(timestamp, us);
    NRF_LOG_INFO("Time set to %llu", timestamp);
}


/**
 * @brief Return current calendar time with Unix timestamp
 * @param[in] timestamp a pointer provided to store the Unix epoch read
 */
void CAL_GetTime(uint64_t * p_timestamp, uint32_t * p_us)
{
    rtc_get_time(p_timestamp, p_us);
}

/**
 * @brief Return rtc instance to get event address
 */
nrfx_rtc_t * CAL_GetRtcInstance(void) {
    return rtc_get_instance();
}


/*
 * Local functions
 */

/* RTC Hardware Abstraction Layer */

/* Set up RTC instance (RTC0 is used by SoftDevice and RTC1 by app_timer module) */
static nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(CAL_RTC_INSTANCE);        /**< RTC instance */
#define POWER2_PRESCALER        (12)//(15)                                 /**< Power of 2 for RTC frequency (15 is 32768 Hz */
#define RTC_COUNTER_FREQUENCY   (1 << POWER2_PRESCALER)             /**< RTC counter frequency in Hz (8 - 32768) */
#define COUNTER_TO_SECS(counts) (counts >> POWER2_PRESCALER)        /**< Macro to convert RTC counter value in seconds */
#define SECS_TO_COUNTER(secs)   (secs << POWER2_PRESCALER)          /**< Macro to convert seconds to RTC counter value */

static int64_t time_offset;        /**< UNIX 64 bit epoch in seconds - Current time is time_offset + (RTC counter value converted in seconds) */
static uint32_t tick_offset;        /**< initial microseconds offset converted to tick to be added at each timestamp */

static void rtc_init(void)
{
    // add lfclk request to keep it alive even if SoftDevice is disabled
    nrf_drv_clock_lfclk_request(NULL);

    // re-init time offset at boot
    time_offset = 0;
    tick_offset = 0;
    // configure and enable RTC instance with 1 Hertz tick
    nrfx_rtc_config_t rtc_config = 
    {
        .prescaler          = RTC_FREQ_TO_PRESCALER(RTC_COUNTER_FREQUENCY),
        .interrupt_priority = NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY,
        .tick_latency       = NRFX_RTC_US_TO_TICKS(NRFX_RTC_MAXIMUM_LATENCY_US, NRFX_RTC_DEFAULT_CONFIG_FREQUENCY),
        .reliable           = NRFX_RTC_DEFAULT_CONFIG_RELIABLE,
    };
    APP_ERROR_CHECK(nrfx_rtc_init(&rtc, &rtc_config, rtc_evt_handler));
    nrfx_rtc_overflow_enable(&rtc, true);
    nrfx_rtc_tick_enable(&rtc, true);
    nrfx_rtc_enable(&rtc);
    nrfx_rtc_counter_clear(&rtc);
}

static void rtc_deinit(void)
{
    nrfx_rtc_disable(&rtc);
    nrfx_rtc_uninit(&rtc);
    nrf_drv_clock_lfclk_release();
}

static void rtc_set_time(const uint64_t time, const uint32_t us)
{
    nrfx_rtc_counter_clear(&rtc);
    time_offset = time;

    float usecs = (float)(us);
    float freq  = (float)(RTC_COUNTER_FREQUENCY);
    tick_offset = (uint32_t)(usecs* freq * 0.000001f);
}

static void rtc_get_time(uint64_t * p_timestamp, uint32_t * p_us)
{
    uint32_t counter_value = nrfx_rtc_counter_get(&rtc);
    uint32_t sec = COUNTER_TO_SECS(counter_value);
    *p_timestamp = time_offset + sec;
    uint64_t mod = counter_value - SECS_TO_COUNTER(sec);
    *p_us = (uint32_t)(mod * 1000000ULL / RTC_COUNTER_FREQUENCY);
}

static void rtc_evt_handler(nrfx_rtc_int_type_t type)
{
    //NRF_LOG_DEBUG("RTC event");
    if (type == NRFX_RTC_INT_OVERFLOW)
    {
        //NRF_LOG_DEBUG("RTC Overflow");
        time_offset += COUNTER_TO_SECS((1<<24));
    }
}

static nrfx_rtc_t * rtc_get_instance(void)
{
    return &rtc;
}

/* END OF FILE */