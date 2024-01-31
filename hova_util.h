#pragma once

#include <hardware/pio.h>
#include "tt_pins.h"

inline static void hova_set_input(int val) {
    val = (val << 4) | (val & 0xF);
    gpio_put_masked((0xF << IN0) | (0xFF << IN4), val << IN0);
}

inline static int hova_get_output() {
    int gpio = gpio_get_all();
    int val = ((gpio >> (UIO4 - 8)) & 0xF00) | ((gpio >> (OUT4 - 4)) & 0xF0) | ((gpio >> (nCRST_OUT2 - 2)) & 0xC) | ((gpio >> SDI_OUT0) & 0x3);
    if (val > 2047) val -= 4096;
    return val;
}

void hova_setup(PIO pio);