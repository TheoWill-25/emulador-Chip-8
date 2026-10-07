#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <iomanip>
#include "../Chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <format>
#include <chrono>

struct TextItem{
    TTF_Text* textptr = nullptr;
    float x = 0.0f;
    float y = 0.0f;
};


class Graphic{
    public:
        SDL_Renderer* rend = nullptr;
        SDL_Window* wind = nullptr;
        virtual void init(int w, int h, const char* n);
        void destroy();
        virtual void draw();
    protected:
        std::vector<std::pair<SDL_FRect, SDL_Color >> rects_v;
    private:
        
        const char* name;
        int width;
        int height;

};

class Display: public Graphic{
    public:
        void init(int w, int h, const char* n) override;
        void rectcreate();
        void draw(Chip8State& state);
};

class Debug: public Graphic{
    public:
        void init(int w, int h, const char* n) override;
        void rectcreate();
        size_t addtext(const std::string& content, float x, float y);
        void updateText(size_t index, const std::string& newContent);
        void draw() override;
        void update(const Chip8State& state);
        void cleanup();
    private:
        float font_size = 15.0f;
        std::vector<TextItem> texts_v;
        TTF_Font* font = nullptr;
        TTF_TextEngine* engine = nullptr;
};


void WindowLoop(Display &display, Debug &debug, Chip8 &chip);