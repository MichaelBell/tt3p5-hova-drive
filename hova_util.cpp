#include <pico/stdlib.h>

#include "hova_util.h"

#include "hova.pio.h"

void hova_reset()
{
    hova_set_input(4);
    gpio_put(nRST, 0);

    for (int i = 0; i < 10; ++i) {
        gpio_put(CLK, 0);
        sleep_us(1);
        gpio_put(CLK, 1);
        sleep_us(1);
    }

    gpio_put(nRST, 1);
}

void hova_setup(PIO pio)
{
    // Bottom four UIOs are TT inputs, RP2040 outputs.
    gpio_set_dir_masked(0xFF << UIO0, 0x0F << UIO0);

    hova_reset();

    hova_pio_program_init(pio);
}
