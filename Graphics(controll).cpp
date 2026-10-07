#include "Graphics.hpp"

void Graphic::init(int w, int h, const char* n) {
    width = w;
    height = h;
    name = n;

    wind = SDL_CreateWindow(name, width, height, 0);
    rend = SDL_CreateRenderer(wind, nullptr);
}

void Debug::init(int w, int h, const char* n) {
    Graphic::init(w, h, n);
    
    // Abra a fonte AQUI, após o renderer ser criado
    font = TTF_OpenFont("source/graphics/Oxanium-Bold.ttf", font_size);
    if (!font) {
        std::cout << "Erro ao carregar fonte: " << SDL_GetError() << "\n";
    }

    engine = TTF_CreateRendererTextEngine(rend);

    addtext("pc: 0", 10.0f, 10.0f);
    addtext("mem_ptr: 0: 0", 10.0f, 30.0f);
    addtext("delay_t: 0", 10.0f, 50.0f);
    addtext("sound_t: 0", 10.0f, 70.0f);
    addtext("stk_ptr: 0", 10.0f, 90.0f);
    addtext("op: 0x00", 130.0f, 10.0f);
    addtext("x: 0x00", 130.0f, 30.0f);
    addtext("y: 0x00", 130.0f, 50.0f);
    addtext("nn: 0x00", 130.0f, 70.0f);
    addtext("nnn: 0x0000", 130.0f, 90.0f);

    for(int i = 0; i < 16; ++i) {
        addtext("reg[" + std::to_string(i) + "]: 0", 10.0f, 130.0f + i * 20.0f);
    }
    for(int i = 0; i < 16; ++i) {
        addtext("stack[" + std::to_string(i) + "]: 0", 130.0f, 130.0f + i * 20.0f);
    }

    addtext("cycles: 0", 250.0f, 10.0f);
    addtext("cycles_per_second: \n 0", 250.0f, 30.0f);   
    addtext("frames: 0", 250.0f, 100.0f);
    addtext("frames_per_second: \n 0", 250.0f, 120.0f);
}

void Graphic::destroy() {
    if (rend) SDL_DestroyRenderer(rend);
    if (wind) SDL_DestroyWindow(wind);
}

void Display::rectcreate() {
    rects_v.push_back({{100.0f, 100.0f, 50.0f, 50.0f}, {0x00, 0x00, 0xFF, 0xFF}});
}

void Debug::rectcreate() {
    rects_v.push_back({{100.0f, 100.0f, 50.0f, 50.0f}, {0xFF, 0x00, 0xFF, 0xFF}});
}

void Graphic::draw() {
    for (const auto &rect : rects_v) {
        SDL_SetRenderDrawColor(rend, rect.second.r, rect.second.g, rect.second.b, rect.second.a);
        SDL_RenderFillRect(rend, &rect.first);
    }
}

void Debug::draw() {
    Graphic::draw();

    for (const auto &item : texts_v) {
        if (item.textptr) {
            TTF_DrawRendererText(item.textptr, item.x, item.y);
        }
    }
}

size_t Debug::addtext(const std::string& content, float x, float y) {
    if (!font || !engine) return -1;

    TTF_Text* newText = TTF_CreateText(engine, font, content.c_str(), 0);
    texts_v.push_back({newText, x, y});

    return texts_v.size() - 1;
}

void Debug::updateText(size_t index, const std::string& newContent) {
    if (index < texts_v.size() && texts_v[index].textptr) {
        TTF_SetTextString(texts_v[index].textptr, newContent.c_str(), 0);
    }
}

void Debug::update(const Chip8State& state) {
    
    updateText(0, "pc: " + std::to_string(state.pc));
    updateText(1, "mem_ptr: " + std::to_string(state.mem_ptr));
    updateText(2, "delay_t: " + std::to_string(state.delay_t));
    updateText(3, "sound_t: " + std::to_string(state.sound_t));
    updateText(4, "stk_ptr: " + std::to_string(state.stk_ptr));
    updateText(5, std::format("op: 0x{:02X}", state.instr.op));
    updateText(6, std::format("x: 0x{:02X}", state.instr.x));
    updateText(7, std::format("y: 0x{:02X}", state.instr.y));
    updateText(8, std::format("nn: 0x{:02X}", state.instr.nn));
    updateText(9, std::format("nnn: 0x{:04X}", state.instr.nnn));
    for(int i = 0; i < 16; ++i) {
        updateText(10 + i, "reg[" + std::to_string(i) + "]: " + std::to_string(state.reg[i]));
    }
    for(int i = 0; i < 16; ++i) {
        updateText(26 + i, "stack[" + std::to_string(i) + "]: " + std::to_string(state.stack[i]));
    }
    updateText(42, "cycles: " + std::to_string(state.cycles));
    updateText(43, "cycles_per_second: \n" + std::to_string(state.cycles_per_second));
    updateText(44, "frames: " + std::to_string(state.frames));
    updateText(45, "frames_per_second: \n" + std::to_string(state.frames_per_second));
}

void Debug::cleanup() {
    for (auto& item : texts_v) {
        if (item.textptr) {
            TTF_DestroyText(item.textptr);
        }
    }
    texts_v.clear();

    if (engine) TTF_DestroyRendererTextEngine(engine);
    if (font) TTF_CloseFont(font);
    
    Graphic::destroy();
}



void WindowLoop(Display &display, Debug &debug, Chip8 &chip) {
    SDL_Event evento;
    bool running = true;

    const double CPU_HZ = 500.0;
    const double CPU_PERIOD = 1.0 / CPU_HZ;
    const double FPS = 60.0;
    const double FRAME_PERIOD = 1.0 / FPS;
    std::uint64_t previous_cycles = 0;
    std::uint64_t previous_frames = 0;

    auto last_cycle = std::chrono::steady_clock::now();
    auto last_hz = std::chrono::steady_clock::now();
    auto last_frame = std::chrono::steady_clock::now();
    auto last_fps = std::chrono::steady_clock::now();

    while (running) {

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - last_cycle).count();

        if (elapsed >= CPU_PERIOD) {

            chip.fetch();
            chip.decode();
            chip.execute();

            chip.cycles++;
            last_cycle = now;

            auto hz_now = std::chrono::steady_clock::now();
            double hz_elapsed = std::chrono::duration<double>(hz_now - last_hz).count();

            if (hz_elapsed >= 1.0) {

                chip.cycles_per_second = chip.cycles - previous_cycles;

                previous_cycles = chip.cycles;
                last_hz = hz_now;
            }
        
        
        }
    
        if (std::chrono::duration<double>(now - last_frame).count() >= FRAME_PERIOD) {        
            // Renderização contínua do Display
            SDL_SetRenderDrawColor(display.rend, 0x80, 0x80, 0x80, 0xFF);
            SDL_RenderClear(display.rend);
            display.draw();
            SDL_RenderPresent(display.rend);
            chip.frames++;

            auto fps_now = std::chrono::steady_clock::now();
            double fps_elapsed = std::chrono::duration<double>(fps_now - last_fps).count();

            if (fps_elapsed >= 1.0)
            {
                chip.frames_per_second = chip.frames - previous_frames;

                previous_frames = chip.frames;
                last_fps = fps_now;
            }

            // Renderização contínua do Debug (inclui o texto)
            debug.update(chip.getstate());
            SDL_SetRenderDrawColor(debug.rend, 0x4F, 0x73, 0x8E, 0xFF);
            SDL_RenderClear(debug.rend);
            debug.draw();
            SDL_RenderPresent(debug.rend);
            last_frame = now;
        }

        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                running = false;
            }
        }
    }
}