#include <stdio.h>
#include <string.h>
#include <math.h>
#include "watch.h"
#include "app_mode.h"
#include "app_board.h"
#include "36hr.h"

const int8_t UTC_OFFSET = 4; // set to your current UTC offset to see correct beats time
const uint8_t BEAT_REFRESH_FREQUENCY = 8;

void do_24_hr_clock_mode(void);
void do_36_hr_clock_mode(void);
void do_set_time_mode(void);
void set_time_mode_handle_primary_button(void);
void set_time_mode_handle_secondary_button(void);

void cb_light_pressed(void);
void cb_mode_pressed(void);
void cb_alarm_pressed(void);
void cb_tick(void);
void cb_fast_tick(void);

AppBoard board;
char buf[16] = {0};

/**
 * @brief Zeroes out the application state struct.
 */
void app_init(void) {
    memset(&board, 0, sizeof(board));
}

void app_wake_from_backup(void) {
    // This app does not support BACKUP mode.
}

void app_setup(void) {
    watch_enable_external_interrupts();
    watch_register_interrupt_callback(BTN_MODE, cb_mode_pressed, INTERRUPT_TRIGGER_RISING);
    watch_register_interrupt_callback(BTN_LIGHT, cb_light_pressed, INTERRUPT_TRIGGER_RISING);
    watch_register_extwake_callback(BTN_ALARM, cb_alarm_pressed, true);

    watch_enable_buzzer();
    watch_enable_leds();
    watch_enable_display();

    watch_rtc_disable_all_periodic_callbacks();
    watch_rtc_register_tick_callback(cb_tick);
}

void app_prepare_for_standby(void) {
    // This app does not support standby mode.
}

void app_wake_from_standby(void) {
    // This app does not support standby mode.
}

bool app_loop(void) {
    // play a beep if the mode has changed in response to a user's press of the MODE button
    if (board.mode_changed) {
        // low note for nonzero case, high note for return to clock
        watch_buzzer_play_note(board.mode ? BUZZER_NOTE_C7 : BUZZER_NOTE_C8, 100);
        board.mode_changed = false;
    }

    // If the user is not in clock mode and the mode timeout has expired, 
    // return them to clock mode
    if (board.mode != MODE_SET && board.mode_ticks == 0) {
        board.mode = MODE_24_HR_CLOCK;
        board.mode_changed = true;
    }

    // If the LED is off and should be on, turn it on
    if (board.light_ticks > 0 && !board.led_on) {
        watch_set_led_green();
        board.led_on = true;
    }

    // if the LED is on and should be off, turn it off
    if (board.led_on && board.light_ticks == 0) {
        // unless the user is holding down the LIGHT button, in which case, give them more time.
        if (watch_get_pin_level(BTN_LIGHT)) {
            board.light_ticks = 3;
        } else {
            watch_set_led_off();
            board.led_on = false;
        }
    }

    switch (board.mode) {
        case MODE_36_HR_CLOCK:
            do_36_hr_clock_mode();
            break;
        case MODE_24_HR_CLOCK:
            do_24_hr_clock_mode();
            break;
        case MODE_SET:
            do_set_time_mode();
            break;
        case NUM_MODES:
            // dummy case, just silences a warning
            break;
    }

    board.mode_changed = false;

    return true;
}

void do_24_hr_clock_mode(void) {
    watch_date_time date_time = watch_rtc_get_date_time();
    const char months[12][3] = {"JA", "FE", "MR", "AR", "MA", "JN", "JL", "AU", "SE", "OC", "NO", "dE"};

    watch_display_string((char *)months[date_time.unit.month - 1], 0);
    sprintf(buf, "%2d%2d%02d%02d", date_time.unit.day, date_time.unit.hour, date_time.unit.minute, date_time.unit.second);
    watch_display_string(buf, 2);
    watch_set_colon();
}

void do_36_hr_clock_mode(void) {
    watch_36hr_date_time date_time =  watch_date_time_to_36hr(watch_rtc_get_date_time());
    const char months[12][3] = {"JA", "FE", "MR", "AR", "MA", "JN", "JL", "AU", "SE", "OC", "NO", "dE"};

    watch_display_string((char *)months[date_time.unit.month - 1], 0);
    sprintf(buf, "%2d%2d%02d%02d", date_time.unit.day, date_time.unit.hour, date_time.unit.minute, date_time.unit.second);
    watch_display_string(buf, 2);
    watch_set_colon();
}

void do_set_time_mode(void) {
    watch_date_time date_time = watch_rtc_get_date_time();

    watch_display_string("          ", 0);
    switch (board.page) {
        case 0: // hour
            sprintf(buf, "ST t%2d", date_time.unit.hour);
            break;
        case 1: // minute
            sprintf(buf, "ST t  %02d", date_time.unit.minute);
            break;
        case 2: // second
            sprintf(buf, "ST t    %02d", date_time.unit.second);
            break;
        case 3: // year
            sprintf(buf, "ST d%2d", date_time.unit.year + 20);
            break;
        case 4: // month
            sprintf(buf, "ST d  %02d", date_time.unit.month);
            break;
        case 5: // day
            sprintf(buf, "ST d    %02d", date_time.unit.day);
            break;
    }
    watch_display_string(buf, 0);
    watch_set_pixel(1, 12); // required for T in position 1
}

void set_time_mode_handle_primary_button(void) {
    board.page++;
    if (board.page == 6) board.page = 0;
}

void set_time_mode_handle_secondary_button(void) {
    watch_date_time date_time = watch_rtc_get_date_time();
    const uint8_t days_in_month[12] = {31, 28, 31, 30, 31, 30, 30, 31, 30, 31, 30, 31};

    switch (board.page) {
        case 0: // hour
            date_time.unit.hour = (date_time.unit.hour + 1) % 24;
            break;
        case 1: // minute
            date_time.unit.minute = (date_time.unit.minute + 1) % 60;
            break;
        case 2: // second
            date_time.unit.second = 0;
            break;
        case 3: // year
            // only allow 2021-2030. fix this sometime next decade
            date_time.unit.year = ((date_time.unit.year % 10) + 1);
            break;
        case 4: // month
            date_time.unit.month = ((date_time.unit.month + 1) % 12);
            break;
        case 5: // day
            date_time.unit.day = date_time.unit.day + 1;
            // can't set to the 29th on a leap year. if it's february 29, set to 11:59 on the 28th.
            // and it should roll over.
            if (date_time.unit.day > days_in_month[date_time.unit.month - 1]) {
                date_time.unit.day = 1;
            }
            break;
    }
    watch_rtc_set_date_time(date_time);
}

void cb_mode_pressed(void) {
    board.mode = (board.mode + 1) % NUM_MODES;
    board.mode_changed = true;
    board.mode_ticks = 300;
    board.page = 0;
}

void cb_light_pressed(void) {
    switch (board.mode) {
        case MODE_SET:
            set_time_mode_handle_secondary_button();
            break;
        default:
            board.light_ticks = 3;
            break;
    }
}

void cb_alarm_pressed(void) {
    switch (board.mode) {
        case MODE_SET:
            set_time_mode_handle_primary_button();
            break;
        default:
            break;
    }
}

void cb_tick(void) {
    if (board.light_ticks > 0) {
        board.light_ticks--;
    }
    if (board.mode_ticks > 0) {
        board.mode_ticks--;
    }
}
