#include <pico/stdlib.h>

#include "tt_pins.h"
#include "tt_setup.h"

int main() {
    //set_sys_clock_khz(48000, true);
    sleep_ms(1);

    tt_select_design(54);

    gpio_put(nRST, 1);
    gpio_put(IN0, 1);
    sleep_us(10);

    while (true) {
        gpio_put(CLK, 1);
        sleep_ms(100);
        gpio_put(CLK, 0);
        sleep_ms(100);
    }
}