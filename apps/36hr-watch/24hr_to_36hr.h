#pragma once

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "36hr.h"
#include "watch_rtc.h"

bool is_leap_year(int year) {
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

//
// Note: The switch statements here aren't necessarily readable.
// Please see the proof of concept notebook to understand how these functions
// were created. Switch statements were used in order to avoid arrays, which
// would need to be 2D to be useful as lookups in to_36_hr_date.
//

uint32_t get_num_days_in_month(uint32_t year, uint32_t month) {
  // clang-format off
  switch(month) {
    case 2: return is_leap_year(year) ?  20 : 19; break;
    case 1:
    case 3: 
    case 5: 
    case 7: 
    case 8:
    case 10:
    case 12: return 21; break;
    case 4:
    case 6:
    case 9:
    case 11: return 20; break;
    default: return 0; break;
  }
  // clang-format on
  return 0;
}

uint32_t get_num_hours_in_day(uint32_t year, uint32_t month, uint32_t day) {
  if (day < 1 || day > get_num_days_in_month(year, month)) {
    return 0;
  }

  // clang-format off
  switch (month) {
    case 2: {
      switch(day) {
        // Account for leap years.
        case 19: return 24; break;
        case 20: return 12; break;
        default: return 36; break;
      }
      break;
    }
    case 1:
    case 3: 
    case 5:
    case 7: 
    case 8:
    case 10:
    case 12: {
      switch(day) {
        case 21: return 24; break;
        default: return 36; break;
      }
      break;
    }
    case 4:
    case 6:
    case 9:
    case 11: return 36; break;
    default: return 0; break;
  }
  // clang-format on

  return 0;
}

watch_36hr_date_time watch_date_time_to_36hr(watch_date_time date) {
  if (date.unit.day == 0) {
    watch_36hr_date_time invalid_date = {
        .second = 0, .minute = 0, .hour = 0, .day = 0, .month = 0, .year = 0};
    return invalid_date;
  }
  // Calculate the total number of hours that have passed from the beginning of
  // the current month until the current hour of the current day.
  uint32_t total_hrs_elapsed = ((date.unit.day - 1) * 24) + date.unit.hour;
  uint32_t days_elapsed = (uint32_t)floor(total_hrs_elapsed / 36);
  uint32_t day_converted = days_elapsed + 1;
  uint32_t hour_converted =
      (total_hrs_elapsed - (days_elapsed * 36)) %
      get_num_hours_in_day(date.unit.year, date.unit.month, day_converted);

  watch_36hr_date_time converted_date = {.second = date.unit.second,
                                         .minute = date.unit.minute,
                                         .hour = hour_converted,
                                         .day = day_converted,
                                         .month = date.unit.month,
                                         .year = date.unit.year};

  return converted_date;
}
