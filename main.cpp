#include <pico/stdlib.h>
#include <hardware/sync.h>

#include <array>
#include <cstdio>

#include "hovalaag.h"
#include "tt_setup.h"

#define DESIGN_NUM 22

const uint32_t hello[] = {
    //ALU- A- B- C- D W- F- PC O I X K----- L-----
    0b0000'00'00'00'0'00'00'01'0'0'0'000000'000001,  // JMP 1
    0b0000'11'00'00'0'00'00'00'0'0'0'000000'000000,  // A=IN1
    0b0000'11'00'00'0'10'00'00'0'0'0'000000'000000,  // W=A, A=IN1
    0b0000'11'00'00'0'10'00'00'1'0'0'000000'000000,  // OUT1=W, W=A, A=IN1
    0b0000'11'00'00'0'10'00'00'1'0'0'000000'000000,  // OUT1=W, W=A, A=IN1
    0b0000'11'00'00'0'10'00'00'1'0'0'000000'000000,  // OUT1=W, W=A, A=IN1
    0b0000'01'00'00'0'10'00'00'1'0'0'000000'000000,  // OUT1=W, W=A, A=0
    0b0000'00'00'00'0'10'00'00'1'0'0'000000'000000,  // OUT1=W, W=A
    0b0000'00'00'00'0'00'00'00'1'0'0'000000'000000   // OUT1=W
};
const std::array<uint16_t, 5> hello_in = { 0b01110110, 0b11111001, 0b00111000, 0b10111000, 0b00111111 };

#include "aoc2020_1_1.h"
#include "aoc2020_5_2.h"

int main() {
    set_sys_clock_khz(133000, true);

    stdio_init_all();
    while (!stdio_usb_connected());

    printf("Selecting Hovalaag\n");
    sleep_ms(500);

    tt_select_design(DESIGN_NUM);
    printf("Selected\n");
    sleep_ms(500);

    Hovalaag hova;

    int program_len = sizeof(hello) / sizeof(hello[0]);
    #if 0
    hova.copy_program(hello, program_len);
    printf("Program loaded\n");

    printf("Run hello\n");
    hova.reset_program(hello_in.begin(), hello_in.end());

    int pc = 0;
    while (pc < program_len) {
        pc = hova.execute_one();
        if (pc >= 3) sleep_ms(500);
    }

    //while (1) __wfe();
    #endif

    //sleep_ms(2000);

#if 1
    program_len = sizeof(aoc2020_1_1_prog) / sizeof(aoc2020_1_1_prog[0]);
    hova.copy_program(aoc2020_1_1_prog, program_len);
    printf("Program loaded\n");

    printf("Run AoC2020 1-1\n");
    hova.reset_program(aoc2020_1_1_in.begin(), aoc2020_1_1_in.end());
    absolute_time_t start_time = get_absolute_time();
    hova.run_until_out1_len(2);
    printf("Executed %d instructions in %.3fms\n", hova.get_num_executed(), absolute_time_diff_us(start_time, get_absolute_time()) * 0.001f);
    for (auto it = hova.out1_begin(); it != hova.out1_end(); ++it) {
        printf("%d ", *it);
    }
    printf("\n");
#endif

    program_len = sizeof(aoc2020_5_2_prog) / sizeof(aoc2020_5_2_prog[0]);
    hova.copy_program(aoc2020_5_2_prog, program_len);
    printf("Program loaded\n");

    printf("Run AoC2020 5-2\n");
    hova.reset_program(aoc2020_5_2_in.begin(), aoc2020_5_2_in.end());
    start_time = get_absolute_time();
    hova.run_until_out1_len(1);
    printf("Executed %d instructions in %.3fms\n", hova.get_num_executed(), absolute_time_diff_us(start_time, get_absolute_time()) * 0.001f);
    for (auto it = hova.out1_begin(); it != hova.out1_end(); ++it) {
        printf("%d ", *it);
    }
    printf("\n");

    while (1) __wfe();
}