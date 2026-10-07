#include "Chip8.hpp"
#include "font.hpp"

Chip8State Chip8::getstate() const {
    Chip8State state;

    state.pc = pc;
    state.mem_ptr = mem_ptr;
    for (int i = 0; i < 16; ++i) {
        state.reg[i] = reg[i];
        state.stack[i] = stack[i];
    }
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < 32; ++j) {
            state.screen[i][j] = screen[i][j];
        }
    }
    state.stk_ptr = stk_ptr;
    state.delay_t = delay_t;
    state.sound_t = sound_t;
    state.instr.op = instr.op;
    state.instr.x = instr.x;
    state.instr.y = instr.y;
    state.instr.n = instr.n;
    state.instr.nn = instr.nn;
    state.instr.nnn = instr.nnn;
    state.cycles = cycles;
    state.cycles_per_second = cycles_per_second;
    state.frames = frames;
    state.frames_per_second = frames_per_second;

    return state;
}

void Chip8::updateTimers() {

    if (delay_t > 0) {
        delay_t--;
    }

    if (sound_t > 0) {
        sound_t--;
    }
}

void Chip8::loadfont(){
    int font_i = 0;
    for(std::uint16_t i = 0x050; i < 0x0A0; i++){
        memory[i] = fontset[font_i];
        font_i++;
    }
};

Chip8::Chip8() : gen(rd()), dist(0, 255) {}

void Chip8::init(){
    clear(memory);
    clear(reg);
    clear(stack);
    clear(key);
    for(int i = 0; i < 64; i++){
        clear(screen[i]);
    }

    mem_ptr = 0;
    pc = 0x200;
    stk_ptr = 0;
    delay_t = 0;
    sound_t = 0;
    cycles = 0;
    cycles_per_second = 0;
    frames = 0;
    frames_per_second = 0;

    loadfont();

};

void Chip8::loadROM(const std::vector<uint8_t> &rom){
    if(rom.size() <= 0xE00){
        for(std::size_t i = 0; i< rom.size(); i++){
            memory[pc + i] = rom[i];
        }
    }
    else {std::cout << "error\n";}
    
};

void Chip8::fetch(){
    opcode = memory[pc];
    opcode = opcode << 8;
    opcode += memory[pc + 1];
    pc += 2;
};

void Chip8::decode(){
    instr.op = opcode >> 12;
    instr.x = (opcode >> 8) & 0xF;
    instr.y = (opcode >> 4) & 0xF;
    instr.n = opcode & 0xF;
    instr.nn = opcode & 0xFF;
    instr.nnn = opcode & 0xFFF;
};

void Chip8::execute(){
    std::uint8_t op = instr.op;
    std::uint8_t  x = instr.x;
    std::uint8_t  y= instr.y;
    std::uint8_t n = instr.n;
    std::uint8_t nn = instr.nn;
    std::uint16_t nnn = instr.nnn;
    std::uint16_t result;
    std::uint8_t C;
    std::uint8_t D;
    std::uint8_t U;
    int rand;
    bool pressed;
    switch (op)
    {
    case 0x0:
        switch (nn)
        {
        case 0xE0: 
            for(auto &i : screen){
                for(auto &j : i){
                    j = false;
                }
            }
            break;
        case 0xEE: 
            if(stk_ptr > 0){
                stk_ptr--;
                pc = stack[stk_ptr];
            }
            break;
        
        default:
            break;
        }
        break;
    case 0x1: pc = nnn; break;
    case 0x2:
        if(stk_ptr < 16)
        {
            stack[stk_ptr] = pc;
            pc = nnn;
            stk_ptr++;
        }
        break;
    case 0x3: pc += (reg[x] == nn) ? 2: 0; break;
    case 0x4: pc += (reg[x] != nn) ? 2: 0; break;
    case 0x5:
        pc += (reg[x] == reg[y]) ? 2: 0; break;
    case 0x6: reg[x] = nn; break;
    case 0x7: reg[x] += nn; break;
    case 0x8: 
        switch (n)
        {
        case 0x0: reg[x] = reg[y]; break;
        case 0x1: reg[x] = reg[x] | reg[y]; break;
        case 0x2: reg[x] = reg[x] & reg[y]; break;
        case 0x3: reg[x] = reg[x] ^ reg[y]; break;
        case 0x4: 
            result = reg[x] + reg[y]; 
            reg[0xF] = (result> 0xFF); 
            reg[x] = result; 
            break;
        case 0x5:
            reg[0xF] = !(reg[x] < reg[y]);
            reg[x] = reg[x] - reg[y];
            break;
        case 0x6: 
            reg[0xF] = reg[x] & 0x01;
            reg[x] = reg[x] >> 1; 
            break;
        case 0x7: 
            reg[0xF] = !(reg[y] < reg[x]);
            reg[x] = reg[y] - reg[x]; 
            break;
        case 0xE:
            reg[0xF] = ((reg[x] & 0x80) == 0x80);
            reg[x] = reg[x] << 1; 
            break;
        
        default:break;
        }
        break;
    case 0x9: pc += (reg[x] != reg[y]) ? 2 : 0; break;
    case 0xA: mem_ptr = nnn; break;
    case 0xB: pc = nnn + reg[0]; break;
    case 0xC: rand = dist(gen); reg[x] = rand & nn; break;
    case 0xD:
        reg[0xF] = 0;
        for (int row = 0; row < instr.n; row++) {
            uint8_t sprite = memory[mem_ptr + row];
            for (int col = 0; col < 8; col++) {
                if ((sprite & (0x80 >> col)) != 0) {
                    int px = (reg[instr.x] + col) % 64;
                    int py = (reg[instr.y] + row) % 32;
                    if (screen[px][py]) {reg[0xF] = 1;}

                    screen[px][py] ^= 1;
                }
            }
        }
        break;
    case 0xE:
        switch (nn)
        {
        case 0x9E: pc += (key[reg[x]]) ? 2 : 0; break;
        case 0xA1: pc += (!key[reg[x]]) ? 2 : 0; break;
        default: break;
        }
        break;
    case 0xF:
        switch (nn)
        {
        case 0x07: reg[x] = delay_t; break;
        case 0x0A: 
            pressed = false;
            for(int i = 0; i < 16; i++){
                if(key[i]) {reg[x] = i;  pressed = true; break;}
            }
            pc -= (!pressed) ? 2 : 0;
            break;
        case 0x15: delay_t = reg[x]; break;
        case 0x18: sound_t = reg[x]; break;
        case 0x1E: mem_ptr += reg[x]; break;
        case 0x29: mem_ptr = 0x050 + (reg[x] * 5); break;
        case 0x33: 
            C = reg[x] / 100;
            D = (reg[x] / 10) % 10;
            U = reg[x] % 10;
            memory[mem_ptr] = C;
            memory[mem_ptr + 1] = D;
            memory[mem_ptr + 2] = U;
            break;
        case 0x55: for(int i = 0; i< x+1; i++) {memory[mem_ptr+i] = reg[i];} break;
        case 0x65: for(int i = 0; i< x+1; i++) {reg[i] = memory[mem_ptr+i] ;} break;
        default: break;
        }
        break;
    default: break;
    }
};
