#pragma once

#include "watch_rtc.h"
#include <stdint.h>

typedef struct {
  uint32_t second; // 0-59
  uint32_t minute; // 0-59
  uint32_t hour;   // 0-36
  uint32_t day;    // 1-20
  uint32_t month;  // 1-12
  uint32_t year;   // 0-63 (representing 2020-2083)
} watch_36hr_date_time;

watch_36hr_date_time watch_date_time_to_36hr(const watch_date_time time);
