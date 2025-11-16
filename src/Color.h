#pragma once

#include <cstdint>

#ifdef USE_SDL
#include <SDL2/SDL_pixels.h>
#else
// Define SDL_Color when SDL is not available
struct SDL_Color {
    uint8_t r, g, b, a;
};
#endif

struct Color {
    uint8_t r, g, b, a;
    
    constexpr Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) 
        : r(r), g(g), b(b), a(a) {}
    
    SDL_Color toSDLColor() const { return {r, g, b, a}; }
    
    // Predefined colors
    static constexpr Color White() { return {255, 255, 255, 255}; }
    static constexpr Color Gray() { return {200, 200, 200, 255}; }
    static constexpr Color Yellow() { return {255, 255, 100, 255}; }
    static constexpr Color LightGreen() { return {180, 255, 180, 255}; }
    static constexpr Color LightBlue() { return {200, 200, 255, 255}; }
    static constexpr Color DarkBackground() { return {40, 40, 60, 255}; }
};
