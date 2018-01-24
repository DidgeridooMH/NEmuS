#ifndef NEMUS_SCREEN_H
#define NEMUS_SCREEN_H

#include <SDL.h>
#include "../Core/PPU.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

namespace nemus {
    namespace ui {
        class Screen {
        private:
            nemus::core::PPU *m_ppu = nullptr;

            SDL_Window *m_window = nullptr;
            SDL_Renderer *m_renderer = nullptr;

            SDL_Texture *m_pixelBuffer = nullptr;

            bool m_quit = false;

            void renderScreen();

        public:
            Screen(nemus::core::PPU *ppu);

            ~Screen();

            void update();

            bool getQuit() { return m_quit; }

        };
    }
}

#endif
