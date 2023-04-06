#pragma once

#include <stdint.h>
#include "watch_rtc.h"

typedef union {
    struct {
        uint32_t second : 6;    // 0-59
        uint32_t minute : 6;    // 0-59
        uint32_t hour : 5;      // 0-36
        uint32_t day : 5;       // 1-20
        uint32_t month : 4;     // 1-12
        uint32_t year : 6;      // 0-63 (representing 2020-2083)
    } unit;

    /**
     * The bit-packed value as expected by the RTC peripheral's CLOCK register.
     */
    uint32_t reg;

} watch_36hr_date_time;

watch_36hr_date_time watch_date_time_to_36hr(const watch_date_time time);

