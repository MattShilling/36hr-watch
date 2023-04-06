#pragma once

/**
 * @brief Internal application data.
 */
typedef struct AppBoard {
    /**
     * Current mode.
     */
    AppMode mode;

    /**
     * Lets us perform one-time setup for a given mode.
     */
    bool mode_changed;

    /**
     * Timeout for the SET mode (returns to 24hr clock after timeout expires).
     */
    uint16_t mode_ticks;

    /**
     * Timeout for the light.
     */
    uint8_t light_ticks;

    /**
     * Indicates that the LED is on.
     */
    bool led_on;

    /**
     * Tracks the current page in log, prefs or settings.
     */
    uint8_t page;

    /**
     * Lets us see when the second changed, for subsecond timing.
     */
    uint8_t last_second;

    /**
     * A value from 0 to (BEAT_REFRESH_FREQUENCY - 1) indicating the fractional second.
     */
    uint8_t subsecond;
} AppBoard;
