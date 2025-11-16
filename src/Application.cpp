#include "Application.h"
#include "Constants.h"
#include <iostream>
#include <cerrno>
#include <cstring>

Application::Application() = default;

Application::~Application() {
    cleanup();
}

bool Application::initialize() {
    // Configure before SDL init / window creation (video driver on these devices is finicky)
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "mali");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window_ = SDL_CreateWindow("Dropbear SSH",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Display::WIDTH, Display::HEIGHT, SDL_WINDOW_SHOWN);
    if (!window_) { sdlFail("SDL_CreateWindow"); return false; }

    sdl_renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl_renderer_) { sdlFail("SDL_CreateRenderer"); return false; }

    font_ = TTF_OpenFont("res/arial.ttf", LogDisplay::FONT_SIZE);
    if (!font_) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Initialize managers
    network_manager_ = std::make_unique<NetworkManager>();
    dropbear_manager_ = std::make_unique<DropbearManager>(
        [this](const std::string& line) { pushLogLine(line); }
    );
    renderer_ = std::make_unique<Renderer>(sdl_renderer_, font_);

    initController();
    
    // Start Dropbear
    dropbear_manager_->start();
    
    // Initial IP snapshot
    refreshIPAddrs();

    running_ = true;
    return true;
}

void Application::run() {
    SDL_Event e;
    while (running_) {
        while (SDL_PollEvent(&e)) {
            handleEvent(e);
        }

        // Update IPs periodically in case WLAN/ETH comes up later
        Uint32 now = SDL_GetTicks();
        if (now - last_ip_refresh_ms_ >= Network::IP_REFRESH_PERIOD_MS) {
            refreshIPAddrs();
            last_ip_refresh_ms_ = now;
        }

        dropbear_manager_->pumpLogs();
        renderer_->render(ip_addrs_, users_, log_lines_);
        SDL_Delay(Display::FRAME_DELAY_MS);
    }
}

void Application::cleanup() {
    dropbear_manager_.reset();
    renderer_.reset();
    network_manager_.reset();
    cleanupSDLResources();
}

void Application::cleanupSDLResources() {
    if (controller_) {
        SDL_GameControllerClose(controller_);
        controller_ = nullptr;
    }
    if (font_) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
    if (sdl_renderer_) {
        SDL_DestroyRenderer(sdl_renderer_);
        sdl_renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}

void Application::initController() {
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller_ = SDL_GameControllerOpen(i);
            if (controller_) {
                std::cout << "Found controller: " << SDL_GameControllerName(controller_) << std::endl;
                break;
            }
        }
    }
    if (!controller_) {
        std::cout << "Warning: No controller found" << std::endl;
    }
}

void Application::handleEvent(const SDL_Event& e) {
    switch (e.type) {
        case SDL_QUIT:
            running_ = false;
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            if (controller_) {
                bool startPressed = SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_START);
                bool backPressed  = SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_BACK);
                if (startPressed && backPressed) running_ = false;
            }
            break;
        default:
            break;
    }
}

void Application::refreshIPAddrs() {
    auto addrs = network_manager_->getIPv4Addresses();
    if (addrs.empty() && ip_addrs_.empty()) {
        // Only log error on first attempt
        pushLogLine(std::string("getifaddrs failed: ") + strerror(errno));
    }
    ip_addrs_ = std::move(addrs);
    
    // Also get system users (only on first refresh to avoid repeated reads)
    if (users_.empty()) {
        users_ = network_manager_->getSystemUsers();
        if (!users_.empty()) {
            pushLogLine("System users found: " + std::to_string(users_.size()));
        }
    }
}

void Application::pushLogLine(const std::string& line) {
    log_lines_.push_back(line);
    if (log_lines_.size() > LogDisplay::MAX_LINES) {
        // Trim oldest
        log_lines_.erase(log_lines_.begin(),
                        log_lines_.begin() + (log_lines_.size() - LogDisplay::MAX_LINES));
    }
}

void Application::sdlFail(const char* what) {
    std::cerr << what << " Error: " << SDL_GetError() << std::endl;
}
