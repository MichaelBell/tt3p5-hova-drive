#pragma once

#include <cstdint>
#include <deque>

#include "hardware/pio.h"

class Hovalaag {
public:
    Hovalaag();

    void copy_program(const uint32_t* program_in, int len);

    template<class In1It>
    void reset_program(In1It in1_first, In1It in1_last);

    template<class In1It, class In2It>
    void reset_program(In1It in1_first, In1It in1_last, In2It in2_first, In2It in2_last);

    int execute_one();
    void run_until_out1_len(int expected_len);

    int get_num_executed() { return executed; }

    uint16_t* out1_begin() const { return out1_start; }
    uint16_t* out1_end() const { return out1; }
    uint16_t* out2_begin() const { return out2_start; }
    uint16_t* out2_end() const { return out2; }

private:
    void run_instr(uint32_t instr);
    void reset();

    // PIO configuration
    const PIO pio = pio0;
    static constexpr uint sm_out = 0;
    static constexpr uint sm_in = 1;

    // Hovalaag state
    static constexpr int MAX_PROGRAM_LEN = 256;
    uint32_t program[MAX_PROGRAM_LEN];
    int program_len = 0;
    int pc = 0;
    int executed = 0;

    static constexpr int FIFO_LEN = 8192;
    uint16_t inout1[FIFO_LEN];
    uint16_t inout2[FIFO_LEN];
    uint16_t* in1 = &inout1[0];
    uint16_t* out1 = &inout1[0];
    uint16_t* out1_start = &inout1[0];
    uint16_t* in2 = &inout2[0];
    uint16_t* out2_start = &inout2[0];
    uint16_t* out2 = &inout2[0];
};

template <class In1It>
inline void Hovalaag::reset_program(In1It in1_first, In1It in1_last)
{
    in1 = &inout1[0];
    out1 = in1;
    for (auto it = in1_first; it != in1_last; ++it) {
        *out1++ = *it;
    }
    *out1++ = 0;
    in2 = &inout2[0];
    out2 = in2;
    *out2 = 0;
    reset();
}

template <class In1It, class In2It>
inline void Hovalaag::reset_program(In1It in1_first, In1It in1_last, In2It in2_first, In2It in2_last)
{
    in1 = &inout1[0];
    out1 = in1;
    for (auto it = in1_first; it != in1_last; ++it) {
        *out1++ = *it;
    }
    *out1++ = 0;
    in2 = &inout2[0];
    out2 = in2;
    for (auto it = in2_first; it != in2_last; ++it) {
        *out2++ = *it;
    }
    *out2++ = 0;
    reset();
}
