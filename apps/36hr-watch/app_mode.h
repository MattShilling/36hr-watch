#pragma once

typedef enum AppMode {
    /**
     * @brief Displays month, day and current 36 hour time.
     */
    MODE_36_HR_CLOCK = 0,

    /**
     * @brief Displays month, day and current 24 hour time.
     */
    MODE_24_HR_CLOCK,

    /**
     * @brief Set time and date.
     */
    MODE_SET,

    /**
     * @brief Last item in the enum, it's the number of cases.
     */
    NUM_MODES
} AppMode;
