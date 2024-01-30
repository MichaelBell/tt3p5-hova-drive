#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include "tt_pins.h"

int tt_select_design(int idx)
{
    // Ensure all pins are set to safe defaults.
    gpio_set_dir_all_bits(0);
    gpio_init_mask(0x3FFFFFFF);

    // Set CLK, MUX sel, enable and inc low, reset high
    gpio_put_all((1 << nRST) | (1 << nCRST_OUT2));

    // Enable non-muxed signals, this sets the mux dir
    gpio_set_dir_all_bits((1 << CLK) | (1 << nRST) | (1 << HK_CSB) | (1 << CTRL_ENA));
    sleep_us(10);

    // Enable muxed signals
    gpio_set_dir_all_bits((1 << CLK) | (1 << nRST) | (1 << HK_CSB) | (1 << CTRL_ENA) | (1 << nCRST_OUT2) | (1 << CINC_OUT3));
    sleep_us(10);

    // Mux control reset
    gpio_put(nCRST_OUT2, 0);
    sleep_us(100);
    gpio_put(nCRST_OUT2, 1);
    sleep_us(10);

    // Mux select
    for (int i = 0; i < idx; ++i) {
        gpio_put(CINC_OUT3, 1);
        sleep_us(1);
        gpio_put(CINC_OUT3, 0);
        sleep_us(1);
    }

    // Enable design
    gpio_put(CTRL_ENA, 1);
    sleep_us(1);

    // Set pull ups on nCRST / CINC and change all muxed pins to inputs, and project inputs to RP2040 outputs
    gpio_set_pulls(nCRST_OUT2, true, false);
    gpio_set_pulls(CINC_OUT3, true, false);
    gpio_set_dir_all_bits((1 << CLK) | (1 << nRST) | (1 << HK_CSB) | (1 << CTRL_ENA) |
                          (0xF << IN0) | (0xF << IN4));
    sleep_us(1);

    // Switch mux
    gpio_put(HK_CSB, 1);
    sleep_us(10);

    // Leave design in reset
    gpio_put(nRST, 0);
    sleep_us(10);
}