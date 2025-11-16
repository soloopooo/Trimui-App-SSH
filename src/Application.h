#pragma once

#include "NetworkManager.h"
#include "DropbearManager.h"
#include "Renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <vector>

class Application {
public:
    Application();
    ~Application();

    // Delete copy operations
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    bool initialize();
    void run();

private:
    void cleanup();
    void cleanupSDLResources();
    void initController();
    void handleEvent(const SDL_Event& e);
    void refreshIPAddrs();
    void pushLogLine(const std::string& line);
    
    static void sdlFail(const char* what);

    // SDL resources
    SDL_Window* window_ = nullptr;
    SDL_Renderer* sdl_renderer_ = nullptr;
    TTF_Font* font_ = nullptr;
    SDL_GameController* controller_ = nullptr;

    // Managers
    std::unique_ptr<NetworkManager> network_manager_;
    std::unique_ptr<DropbearManager> dropbear_manager_;
    std::unique_ptr<Renderer> renderer_;

    // State
    bool running_ = false;
    std::vector<std::string> ip_addrs_;
    std::vector<std::string> users_;
    std::vector<std::string> log_lines_;
    Uint32 last_ip_refresh_ms_ = 0;
};
