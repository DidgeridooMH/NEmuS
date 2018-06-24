#include <iostream>
#include "Screen.h"

nemus::ui::Screen::Screen(nemus::core::PPU *ppu) {
    m_ppu = ppu;

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }

    m_window = SDL_CreateWindow("NEmuS Alpha Build", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(m_window == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(m_renderer == nullptr) {
        SDL_DestroyWindow(m_window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    m_pixelBuffer = SDL_CreateTexture(m_renderer,
                                      SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
                                      SCREEN_WIDTH, SCREEN_HEIGHT);

}

nemus::ui::Screen::~Screen() {
    SDL_DestroyTexture(m_pixelBuffer);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void nemus::ui::Screen::update() {
    SDL_Event event;

    SDL_UpdateTexture(m_pixelBuffer, NULL, m_ppu->getPixels(), SCREEN_WIDTH * sizeof(Uint32));

    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            m_quit = true;
            break;
    }
}

void nemus::ui::Screen::render() {
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_pixelBuffer, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}