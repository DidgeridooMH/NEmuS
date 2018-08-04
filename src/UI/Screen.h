#ifndef NEMUS_SCREEN_H
#define NEMUS_SCREEN_H

#include <QMainWindow>
#include "SettingsState.h"
#include "../Core/PPU.h"
#include "../Core/Input.h"

#define SCREEN_OFFSET 21
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH  256

namespace nemus {
    class NES;
}

namespace nemus::ui {
    class Screen : public QMainWindow{

    Q_OBJECT

    private:
        NES*       m_nes = nullptr;
        core::PPU* m_ppu = nullptr;
        core::Input* m_input = nullptr;

        QMenu*   m_fileMenu;
        QAction* m_loadRomAction;
        QAction* m_settingsAction;
        QAction* m_exitAction;

        SettingsState* m_state;

        bool m_quit = false;
        std::chrono::system_clock::time_point m_oldTime;

        void applySettings();

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;

    public:
        Screen(core::PPU *ppu, NES* nes, core::Input* input, QWidget* parent);
        ~Screen();

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
        void openSettings();
    };
}

#endif
