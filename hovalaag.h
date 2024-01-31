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
    void run_until_out1_len(size_t expected_len);

    int get_num_executed() { return executed; }

    std::deque<int16_t>::iterator out1_begin() { return out1.begin(); }
    std::deque<int16_t>::iterator out1_end() { return out1.end(); }
    std::deque<int16_t>::iterator out2_begin() { return out2->begin(); }
    std::deque<int16_t>::iterator out2_end() { return out2->end(); }

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
    std::deque<int16_t> in1;
    std::deque<int16_t> in2;
    std::deque<int16_t> out1;
    std::deque<int16_t> _out2;
    std::deque<int16_t>* out2 = &_out2;
};

template <class In1It>
inline void Hovalaag::reset_program(In1It in1_first, In1It in1_last)
{
    in1.clear();
    std::copy(in1_first, in1_last, std::back_inserter(in1));
    in2.clear();
    out2 = &in2;
    reset();
}

template <class In1It, class In2It>
inline void Hovalaag::reset_program(In1It in1_first, In1It in1_last, In2It in2_first, In2It in2_last)
{
    in1.clear();
    std::copy(in1_first, in1_last, std::back_inserter(in1));
    in2.clear();
    std::copy(in1_first, in1_last, std::back_inserter(in2));
    out2 = &_out2;
    reset();
}
