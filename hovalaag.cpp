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
    while (out1.size() < expected_len) {
        if (pc >= program_len) {
            printf("PC=%d is out of range\n", pc);
            return;
        }
        run_instr(program[pc]);
    }
}

void Hovalaag::run_instr(uint32_t instr)
{
    pio_sm_put(pio, sm_out, instr);

    int in1_val = in1.empty() ? 0 : in1.front();
    int in2_val = in2.empty() ? 0 : in2.front();

    const int stat = pio_sm_get_blocking(pio, sm_in);
    //printf("Stat: %02x\n", stat);

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
    
    int16_t out = pio_sm_get_blocking(pio, sm_in);
    if (out > 2047) out -= 4096;
    //printf("OUT: %d\n", out);

    if (stat & 0x10) out1.push_back(out);
    else if (stat & 0x20) out2->push_back(out);
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
