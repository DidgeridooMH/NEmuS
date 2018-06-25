#ifndef NEMUS_SCREEN_H
#define NEMUS_SCREEN_H

#include "../Core/PPU.h"
#include <QMainWindow>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

namespace nemus {
    class NES;
}

namespace nemus {
    namespace ui {
        class Screen : public QMainWindow{

        Q_OBJECT

        private:
            core::PPU *m_ppu = nullptr;
            NES* m_nes = nullptr;

            bool m_quit = false;

            QMenu* m_fileMenu;
            QAction* m_loadRomAction;
            QAction* m_exitAction;

            std::chrono::system_clock::time_point m_oldTime;

        public:
            Screen(core::PPU *ppu, NES* nes, QWidget* parent);

            void updateWindow();

            void updateFPS();

            bool getQuit() { return m_quit; }

            void create_menu();

            void paintEvent(QPaintEvent *event);
            void closeEvent(QCloseEvent *event);

        protected:
        #ifndef QT_NO_CONTEXTMENU
            void contextMenuEvent(QContextMenuEvent* event) override;
        #endif

        signals:

        public slots:
            void openRom();
        };
    }
}

#endif
