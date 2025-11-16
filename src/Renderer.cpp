#include "Renderer.h"
#include "Constants.h"
#include <algorithm>

Renderer::Renderer(SDL_Renderer* renderer, TTF_Font* font)
    : renderer_(renderer), font_(font) {
}

void Renderer::render(const std::vector<std::string>& ipAddrs,
                      const std::vector<std::string>& users,
                      const std::vector<std::string>& logLines) {
    clearScreen();
    
    int y = 30;
    y = renderTitle(y);
    y = renderIPAddresses(y, ipAddrs);
    y = renderUsers(y, users);
    y = renderLogs(y, logLines);
    renderFooter();

    SDL_RenderPresent(renderer_);
}

void Renderer::clearScreen() {
    const Color bg = Color::DarkBackground();
    SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer_);
}

int Renderer::renderTitle(int y) const {
    renderText(renderer_, font_, "Dropbear SSH Server", 
               Display::WIDTH / 2, y, Color::White(), true);
    return y + 40;
}

int Renderer::renderIPAddresses(int y, const std::vector<std::string>& ipAddrs) const {
    if (ipAddrs.empty()) {
        renderText(renderer_, font_, "IP: (resolving...)", 
                  50, y, Color::Gray(), false);
        y += 24;
    } else {
        for (const auto& ip : ipAddrs) {
            renderText(renderer_, font_, "IP: " + ip, 
                      50, y, Color::LightGreen(), false);
            y += 24;
        }
    }
    return y + 8;
}

int Renderer::renderUsers(int y, const std::vector<std::string>& users) const {
    if (!users.empty()) {
        renderText(renderer_, font_, "System Users:", 50, y, Color::LightBlue(), false);
        y += 24;
        
        for (const auto& user : users) {
            renderText(renderer_, font_, "  " + user, 50, y, Color::Yellow(), false);
            y += 20;
        }
        y += 8;
    }
    return y;
}

int Renderer::renderLogs(int y, const std::vector<std::string>& logLines) const {
    renderText(renderer_, font_, "Logs:", 50, y, Color::LightBlue(), false);
    y += 28;

    const int max_visible = (Display::HEIGHT - y - 50) / LogDisplay::LINE_HEIGHT;
    const int start_idx = std::max(0, static_cast<int>(logLines.size()) - max_visible);

    for (int i = start_idx; i < static_cast<int>(logLines.size()); ++i) {
        renderText(renderer_, font_, logLines[i], 50, y, Color::White(), false);
        y += LogDisplay::LINE_HEIGHT;
    }
    
    return y;
}

void Renderer::renderFooter() const {
    renderText(renderer_, font_, "Press START + SELECT to exit",
               Display::WIDTH / 2, Display::HEIGHT - 40,
               Color::Gray(), true);
}

void Renderer::renderText(SDL_Renderer* renderer, TTF_Font* font,
                          const std::string& text, int x, int y,
                          const Color& color, bool centered) {
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color.toSDLColor());
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect destRect = {x, y, surface->w, surface->h};
    if (centered) destRect.x = x - surface->w / 2;

    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
