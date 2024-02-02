#include <cstring>
#include <cstdio>
#include <pico/stdlib.h>

#include "hovalaag.h"
#include "hova_util.h"

Hovalaag::Hovalaag() {
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

void Hovalaag::run_until_out1_len(size_t expected_len)
{
    // JMP 0 to prime the inputs
    run_instr(0b0000'00'00'00'0'00'00'01'0'0'0'000000'000000);
#if 0
    while (out1.size() < expected_len) {
        if (pc >= program_len) {
            printf("PC=%d is out of range\n", pc);
            return;
        }
        run_instr(program[pc]);
    }
#else
    int in_val;
    bool in2_empty;
    {
        const int in1_val = in1.empty() ? 0 : in1.front();
        in2_empty = in2.empty();
        const int in2_val = in2_empty ? 0 : in2.front();
        in_val = in1_val | (in2_val << 12);
    }

    while (out1.size() < expected_len)
    {
        if (pc >= program_len) {
            printf("PC=%d is out of range\n", pc);
            return;
        } 

        pio_sm_put(pio, sm_out, program[pc]);

        const int stat = pio_sm_get_blocking(pio, sm_in);
        //printf("Stat: %02x\n", stat & 0x33);

        if (stat & 1) {
            in1.pop_front();
            const int in1_val = in1.empty() ? 0 : in1.front();
            in_val = (in_val & 0xFFF000) | in1_val;
        }
        else if (stat & 2) {
            in2.pop_front();
            in2_empty = in2.empty();
            const int in2_val = in2_empty ? 0 : in2.front();
            in_val = (in_val & 0xFFF) | (in2_val << 12);
        }

        pio_sm_put(pio, sm_out, in_val);
        pio_sm_put(pio, sm_in, (stat & 0x30) ? 0 : 1);
        

        pc = pio_sm_get_blocking(pio, sm_in);
        //printf("PC: %02x\n", pc);
        
        if (stat & 0x30) {
            int out = pio_sm_get_blocking(pio, sm_in);
            //pio_sm_put(pio, sm_in, 0);
            
            //out = (out & 0x3) | ((out >> 2) & 0xC) | ((out >> 6) & 0xF0) | ((out >> 14) & 0xF00);
            if (out > 2047) out -= 4096;
            //printf("OUT: %d\n", out);

            if (stat & 0x10) out1.push_back(out);
            else if (stat & 0x20) {
                out2->push_back(out);
                if (in2_empty && !in2.empty()) {
                    in2_empty = false;
                    in_val = (in_val & 0xFFF) | (in2.front() << 12);
                }
            }
        }

        ++executed;       
    }
#endif
}

void Hovalaag::run_instr(uint32_t instr)
{
    pio_sm_put(pio, sm_out, instr);

    int in1_val = in1.empty() ? 0 : in1.front();
    int in2_val = in2.empty() ? 0 : in2.front();

    const int stat = pio_sm_get_blocking(pio, sm_in);
    pio_sm_put(pio, sm_in, (stat & 0x30) ? 0 : 1);
    //printf("Stat: %02x\n", stat & 0x33);

    if (stat & 1) {
        in1.pop_front();
        in1_val = in1.empty() ? 0 : in1.front();
    }
    else if (stat & 2) {
        in2.pop_front();
        in2_val = in2.empty() ? 0 : in2.front();
    }

    pio_sm_put(pio, sm_out, in1_val | (in2_val << 12));

    pc = pio_sm_get_blocking(pio, sm_in);
    //printf("PC: %02x\n", pc);
    
    if (stat & 0x30) {
        int out = pio_sm_get_blocking(pio, sm_in);
        //out = (out & 0x3) | ((out >> 2) & 0xC) | ((out >> 6) & 0xF0) | ((out >> 14) & 0xF00);
        if (out > 2047) out -= 4096;
        //printf("OUT: %d\n", out);

        if (stat & 0x10) out1.push_back(out);
        else if (stat & 0x20) out2->push_back(out);
    }
    ++executed;
    //sleep_ms(100);
}

void Hovalaag::reset()
{
    pc = 0;
    executed = 0;
    out1.clear();
    _out2.clear();
}
