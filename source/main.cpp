#include "Chip8.hpp"
#include "graphics\Graphics.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>


std::vector<std::uint8_t> loadROMFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Nao foi possivel abrir a ROM");
    }

    return std::vector<std::uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}


int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "Erro ao inicializar SDL: " << SDL_GetError() << "\n";
        return -1;
    }

    if (!TTF_Init()) {
        std::cout << "Erro ao inicializar SDL_ttf: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    Chip8 chip;
    chip.init();

    std::vector<std::uint8_t> rom = loadROMFile("jogos_chip_8/pong.ch8");
    chip.loadROM(rom);

    Display display;
    Debug debug;
    display.init(800, 400, "CHIP_8");
    debug.init(400, 480, "DEBUG");

    if (display.wind == nullptr || debug.wind == nullptr)
    {
        std::cout << SDL_GetError() << "\n";
        SDL_Quit();
        return 0;
    }

    //std::cout << "3\n";

    WindowLoop(display, debug, chip);

    //std::cout << "4\n";
    debug.destroy();
    display.destroy();
    SDL_Quit();

    return 0;
}





/*int main(){
    Chip8 chip;
    chip.init();
    std::vector<uint8_t> rom = {0x60, 0x01, 0x61, 0x02, 0xD0, 0x10, 0x00, 0xE0};
    chip.loadROM(rom);
    std::uint16_t result = chip.fetch();
    std::cout << std::hex << result << "\n";
}*/