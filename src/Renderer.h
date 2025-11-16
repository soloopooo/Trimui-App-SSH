#pragma once

#include "Color.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class Renderer {
public:
    Renderer(SDL_Renderer* renderer, TTF_Font* font);
    
    void render(const std::vector<std::string>& ipAddrs,
                const std::vector<std::string>& users,
                const std::vector<std::string>& logLines);

private:
    void clearScreen();
    int renderTitle(int y) const;
    int renderIPAddresses(int y, const std::vector<std::string>& ipAddrs) const;
    int renderUsers(int y, const std::vector<std::string>& users) const;
    int renderLogs(int y, const std::vector<std::string>& logLines) const;
    void renderFooter() const;
    
    static void renderText(SDL_Renderer* renderer, TTF_Font* font,
                          const std::string& text, int x, int y,
                          const Color& color, bool centered);

    SDL_Renderer* renderer_;
    TTF_Font* font_;
};
