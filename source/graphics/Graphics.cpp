#include "Graphics.hpp"

void Graphic::init(int w, int h, const char* n) {
    width = w;
    height = h;
    name = n;

    wind = SDL_CreateWindow(name, width, height, 0);
    rend = SDL_CreateRenderer(wind, nullptr);
}

void Display::init(int w, int h, const char* n){
    Graphic::init(w, h, n);
    
    
    rectcreate();
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
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 32; j++){
            rects_v.push_back({{(12.0f *i) + 10.0f, (12.0f * j) +5.0f, 12.0f, 12.0f}, {0x00, 0x00, 0x00, 0xFF}});
        }
    }  
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

void Display::draw(Chip8State& state){
    for(int i = 0; i< 64; i++) {
        for(int j = 0; j<32; j++){
            SDL_Color color = (state.screen[i][j]) ? SDL_Color{0xFF, 0xFF, 0xFF, 0xFF} : SDL_Color{0x00, 0x00, 0x00, 0xFF};
            SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(rend, &rects_v[i * 32 + j].first);
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
    Chip8State state;
    bool running = true;
    bool old_screen[64][32] = {};
    const double cpu_speed = 800.0;
    const double frame_rate = 60.0;
    std::uint64_t last_cycles = 0;
    std::uint64_t last_frames = 0;
    auto last_timer = std::chrono::steady_clock::now();
    auto last_debug = std::chrono::steady_clock::now();
    auto last_cpu = std::chrono::steady_clock::now();
    auto last_frame = std::chrono::steady_clock::now();
    auto last_stats = std::chrono::steady_clock::now();



    while (running) {

        // =========================
        // EVENTOS
        // =========================

        while (SDL_PollEvent(&evento)) {

            if (evento.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                running = false;
            }
            if(evento.type == SDL_EVENT_KEY_DOWN || evento.type == SDL_EVENT_KEY_UP) {
                bool pressed = (evento.type == SDL_EVENT_KEY_DOWN);
                switch (evento.key.key) {
                    case SDLK_1: chip.key[0x1] = pressed; break;
                    case SDLK_2: chip.key[0x2] = pressed; break;
                    case SDLK_3: chip.key[0x3] = pressed; break;
                    case SDLK_4: chip.key[0xC] = pressed; break;
                    case SDLK_Q: chip.key[0x4] = pressed; break;
                    case SDLK_W: chip.key[0x5] = pressed; break;
                    case SDLK_E: chip.key[0x6] = pressed; break;
                    case SDLK_R: chip.key[0xD] = pressed; break;
                    case SDLK_A: chip.key[0x7] = pressed; break;
                    case SDLK_S: chip.key[0x8] = pressed; break;
                    case SDLK_D: chip.key[0x9] = pressed; break;
                    case SDLK_F: chip.key[0xE] = pressed; break;
                    case SDLK_Z: chip.key[0xA] = pressed; break;
                    case SDLK_X: chip.key[0x0] = pressed; break;
                    case SDLK_C: chip.key[0xB] = pressed; break;
                    case SDLK_V: chip.key[0xF] = pressed; break;
                }

            }
        }

        // =========================
        // CPU DO CHIP-8
        // =========================

        auto nowcpu = std::chrono::steady_clock::now();

        double cpu_elapsed = std::chrono::duration<double>(nowcpu - last_cpu).count();
        int cycles_to_execute = static_cast<int>(cpu_elapsed * cpu_speed);

        double stats_elapsed = std::chrono::duration<double>(nowcpu - last_stats).count();

        if (stats_elapsed >= 1.0) {

            chip.cycles_per_second = static_cast<double>(chip.cycles - last_cycles) / stats_elapsed;
            chip.frames_per_second = static_cast<double>(chip.frames - last_frames) / stats_elapsed;

            last_cycles = chip.cycles;
            last_frames = chip.frames;

            last_stats = nowcpu;
        }

        for (int i = 0; i < cycles_to_execute; i++) {
            chip.fetch();
            chip.decode();
            chip.execute();

            chip.cycles++;

        }

        if (cycles_to_execute > 0) {
            last_cpu = nowcpu;
        }

        // TIMERS DO CHIP-8

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(now - last_timer).count() >= 1.0 / 60.0) {
            chip.updateTimers();
            last_timer = now;
        }

        state = chip.getstate();
        if (std::chrono::duration<double>(now - last_debug).count() >= 0.1) {
            debug.update(state);
            SDL_SetRenderDrawColor(debug.rend, 0x4F, 0x73, 0x8E, 0xFF);
            SDL_RenderClear(debug.rend);

            debug.draw();
            SDL_RenderPresent(debug.rend);
            last_debug = now;
        }

                
        bool changed = false;

        for (int x = 0; x < 64; x++) {
            for (int y = 0; y < 32; y++) {
                if (state.screen[x][y] != old_screen[x][y]) {
                    changed = true;
                    break;
                }
            }
        }

            
        double frame_elapsed = std::chrono::duration<double>(now - last_frame).count();
        if (frame_elapsed >= 1.0 / frame_rate) {

            SDL_SetRenderDrawColor(display.rend, 0x80, 0x80, 0x80, 0xFF);
            SDL_RenderClear(display.rend);

            display.draw(state);
            SDL_RenderPresent(display.rend);

            for (int x = 0; x < 64; x++) {
                for (int y = 0; y < 32; y++) {
                    old_screen[x][y] = state.screen[x][y];
                }
            }

            chip.frames++;

            last_frame = now;
            
        }
    }
}