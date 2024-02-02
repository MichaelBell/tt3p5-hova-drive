#include <cstring>
#include <cstdio>
#include <pico/stdlib.h>
#include <hardware/sync.h>
#include <hardware/structs/bus_ctrl.h>

#include "hovalaag.h"
#include "hova_util.h"

Hovalaag::Hovalaag() {
    hw_set_bits(&bus_ctrl_hw->priority, BUSCTRL_BUS_PRIORITY_PROC1_BITS);
    hova_setup(pio);
}

void Hovalaag::copy_program(const uint32_t *program_in, int len)
{
    memcpy(program, program_in, sizeof(program[0]) * len);
    program_len = len;
}

int Hovalaag::execute_one()
{
    run_instr(program[pc]);
    return pc;
}

void __no_inline_not_in_flash_func(Hovalaag::run_until_out1_len)(int expected_len)
{
    // JMP 0 to prime the inputs
    run_instr(0b0000'00'00'00'0'00'00'01'0'0'0'000000'000000);
#if 0
    while (out1 - out1_start < expected_len) {
        if (pc >= program_len) {
            printf("PC=%d is out of range\n", pc);
            return;
        }
        run_instr(program[pc]);
    }
#else
    while (out1 - out1_start < expected_len)
    {
        if (pc >= program_len) {
            printf("PC=%d is out of range\n", pc);
            return;
        } 

        pio_sm_put(pio, sm_out, program[pc]);

        const int stat = pio_sm_get_blocking(pio, sm_in);
        //printf("Stat: %02x\n", stat & 0x33);

        if (stat & 1) {
            in1++;
            if (in1 == &inout1[FIFO_LEN]) in1 = &inout1[0];
        }
        else if (stat & 2) {
            in2++;
            if (in2 == &inout2[FIFO_LEN]) in2 = &inout2[0];
        }

        pio_sm_put(pio, sm_out, *in1 | (*in2 << 12));
        pio_sm_put(pio, sm_in, (stat & 0x30) ? 0 : 1);
        
        pc = pio_sm_get_blocking(pio, sm_in);
        //printf("PC: %02x\n", pc);
        
        if (stat & 0x30) {
            int out = pio_sm_get_blocking(pio, sm_in);
            //pio_sm_put(pio, sm_in, 0);
            
            //out = (out & 0x3) | ((out >> 2) & 0xC) | ((out >> 6) & 0xF0) | ((out >> 14) & 0xF00);
            if (out > 2047) out -= 4096;
            //printf("OUT: %d\n", out);

            if (stat & 0x10) {
                *out1++ = out;
                if (out1 == &inout1[FIFO_LEN]) out1 = &inout1[0];
            }
            else if (stat & 0x20) {
                *out2++ = out;
                if (out2 == &inout2[FIFO_LEN]) out2 = &inout2[0];
                if (out2 == in2) {
                    printf("Error, FIFO too small\n");
                    pc = 256;
                    return;
                }
                *out2 = 0;
            }
        }

        ++executed;       
    }
#endif
}

void __no_inline_not_in_flash_func(Hovalaag::run_instr)(uint32_t instr)
{
    pio_sm_put(pio, sm_out, instr);

    const int stat = pio_sm_get_blocking(pio, sm_in);
    //printf("Stat: %02x\n", stat & 0x33);
    
    if (stat & 1) {
        in1++;
        if (in1 == &inout1[FIFO_LEN]) in1 = &inout1[0];
    }
    else if (stat & 2) {
        in2++;
        if (in2 == &inout2[FIFO_LEN]) in2 = &inout2[0];
    }

    //printf("IN1: %03x  IN2: %03x\n", *in1, *in2);
    pio_sm_put(pio, sm_out, *in1 | (*in2 << 12));
    pio_sm_put(pio, sm_in, (stat & 0x30) ? 0 : 1);

    pc = pio_sm_get_blocking(pio, sm_in);
    //printf("PC: %02x\n", pc);
    
    if (stat & 0x30) {
        int out = pio_sm_get_blocking(pio, sm_in);
        //out = (out & 0x3) | ((out >> 2) & 0xC) | ((out >> 6) & 0xF0) | ((out >> 14) & 0xF00);
        if (out > 2047) out -= 4096;
        //printf("OUT: %d\n", out);

        if (stat & 0x10) {
            *out1++ = out;
            if (out1 == &inout1[FIFO_LEN]) out1 = &inout1[0];
        }
        else if (stat & 0x20) {
            *out2++ = out;
            if (out2 == &inout2[FIFO_LEN]) out2 = &inout2[0];
            if (out2 == in2) {
                printf("Error, FIFO too small\n");
                pc = 256;
                return;
            }
            *out2 = 0;
        }
    }

    ++executed;
    //sleep_ms(100);
}

void Hovalaag::reset()
{
    pc = 0;
    executed = 0;
    out1_start = out1;
    out2_start = out2;
}
