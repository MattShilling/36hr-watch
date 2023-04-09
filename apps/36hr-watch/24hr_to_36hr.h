#pragma once

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "36hr.h"

bool is_leap_year(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

//
// Note: The switch statements here aren't necessarily readable.
// Please see the proof of concept notebook to understand how these functions were created.
// Switch statements were used in order to avoid arrays, which would need to be 2D to be useful as lookups in to_36_hr_date.
//

uint32_t get_num_days_in_month(uint32_t year, uint32_t month) {
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
    return 0;
}

uint32_t get_num_hours_in_day(uint32_t year, uint32_t month, uint32_t day) {
    if (day < 1 || day > get_num_days_in_month(year, month)) {
        return 0;
    }

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

    return 0;
}

watch_36hr_date_time to_36_hr_date(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second) {
    // Calculate the total number of hours that have passed from the beginning of the current
    // month until the the current day.
    uint32_t hrs_elapsed_to_day = day * 24;
    uint32_t day_converted = floor(hrs_elapsed_to_day / 36);

    // The last day of each 36hr month might not actually have 36hrs in it (a short day).
    // Check if the day is a short day and handle accordingly.
    uint32_t last_day = get_num_days_in_month(year, month);
    bool before_last_day = (day_converted == last_day - 1);
    bool last_day_is_short = (get_num_hours_in_day(year, month, last_day) != 36);
    bool short_day = before_last_day && last_day_is_short;
    if (short_day) {
        day_converted++;
    }

    if (get_num_hours_in_day(year, month, day_converted) == 0) {
        watch_36hr_date_time invalid_date = {
            .second=0,
            .minute=0,
            .hour=0,
            .day=0,
            .month=0,
            .year=0
        };
        return invalid_date;
    }

    // Total number of hours from the beginning of the current month until the current
    // _hour_ of the current day.
    uint32_t total_hrs_elapsed = (day * 24) + hour;
    uint32_t hour_converted = total_hrs_elapsed % get_num_hours_in_day(year, month, day_converted);

    watch_36hr_date_time converted_date = {
        .second=second,
        .minute=minute,
        .hour=hour_converted,
        .day=day_converted,
        .month=month,
        .year=year
    };

    return converted_date;
}