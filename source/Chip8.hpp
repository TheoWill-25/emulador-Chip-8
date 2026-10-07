#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <iomanip>
#include <random>



struct Instruction{
    std::uint8_t op;
    std::uint8_t x;
    std::uint8_t y;
    std::uint8_t n;
    std::uint8_t nn;
    std::uint16_t nnn;
};

struct Chip8State {
    std::uint16_t pc;
    std::uint16_t mem_ptr;
    std::uint8_t reg[16];
    std::uint16_t stack[16];
    std::uint8_t stk_ptr;
    std::uint8_t delay_t;
    std::uint8_t sound_t;
    std::uint64_t cycles;
    std::uint64_t cycles_per_second;
    std::uint64_t frames;
    std::uint64_t frames_per_second;
    bool screen[64][32];
    Instruction instr;
    
};

class Chip8 {
    public:
        Chip8();
        void init();
        void loadfont();
        void loadROM(const std::vector<uint8_t> &rom);
        void fetch();
        void decode();
        void execute();
        void updateTimers();
        Chip8State getstate() const;
        std::uint64_t cycles_per_second;
        std::uint64_t cycles;
        std::uint64_t frames_per_second;
        std::uint64_t frames;
        bool key[16];
        bool screen[64][32];

    private:
    
        std::uint16_t opcode = 0x0;
        Instruction instr = {0, 0, 0, 0, 0, 0};
        
        std::uint8_t delay_t;
        std::uint8_t sound_t;
        std::uint8_t memory[4096];
        std::uint8_t reg[16];
        std::uint8_t stk_ptr;
        std::uint16_t mem_ptr;
        std::uint16_t pc;
        std::uint16_t stack[16];
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_int_distribution<int> dist;
};

template <typename C, std::size_t N>
void clear(C (&data)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        data[i] = 0;
    }
};