#pragma once

#include "hardware/gpio.h"

enum GPIOMap {
    CLK = 0,
    HK_CSB = 1,
    HK_SCK = 2,
    SDI_OUT0 = 3,
    SDO_OUT1 = 4,
    nRST = 5,
    CTRL_ENA = 6,
    nCRST_OUT2 = 7,
    CINC_OUT3 = 8,
    IN0 = 9,
    IN1 = 10,
    IN2 = 11,
    IN3 = 12,
    OUT4 = 13,
    OUT5 = 14,
    OUT6 = 15,
    OUT7 = 16,
    IN4  = 17,
    IN5  = 18,
    IN6  = 19,
    IN7  = 20,
    UIO0 = 21,
    UIO1 = 22,
    UIO2 = 23,
    UIO3 = 24,
    UIO4 = 25,
    UIO5 = 26,
    UIO6 = 27,
    UIO7 = 28,
    RPIO29 = 29,
};

inline static void tt_set_input_byte(int val) {
    val = (val << 4) | (val & 0xF);
    gpio_put_masked((0xF << IN0) | (0xF << IN4), val << IN0);
}

inline static int tt_get_output_byte() {
    int gpio = gpio_get_all();
    return ((gpio >> (OUT4 - 4)) & 0xF0) | ((gpio >> (nCRST_OUT2 - 2)) & 0xC) | ((gpio >> SDI_OUT0) & 0x3);
}
