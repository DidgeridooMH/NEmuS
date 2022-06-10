#include <chrono>
#include <QApplication>
#include <QAction>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMenuBar>
#include <QPainter>
#include <QVBoxLayout>

#include <fmt/printf.h>
#include <fmt/color.h>

#include <quazip.h>
#include <quazipfile.h>

#include "Screen.h"
#include "../Core/NES.h"
#include "Settings.h"

nemus::ui::Screen::Screen(core::PPU *ppu, NES *nes, core::Input *input, QWidget *parent) : QMainWindow(parent)
{
    m_ppu = ppu;
    m_nes = nes;
    m_input = input;

    m_state = new SettingsState(SCALE_1X);

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins(5, 5, 5, 5));
    layout->addWidget(topFiller);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    create_menu();

    std::string title = "NEmuS Alpha";
    setWindowTitle(QString::fromStdString(title));

    applySettings();

    setFocusPolicy(Qt::ClickFocus);
    show();

    m_oldTime = std::chrono::system_clock::now();
}

nemus::ui::Screen::~Screen()
{
    delete m_fileMenu;
    delete m_loadRomAction;
    delete m_settingsAction;
    delete m_exitAction;

    delete m_state;
}

void nemus::ui::Screen::updateWindow()
{
    QApplication::processEvents();
    update();
}

void nemus::ui::Screen::keyPressEvent(QKeyEvent *event)
{
    event->accept();
    switch (event->key())
    {
    case Qt::Key_Z:
        m_input->setButton(BUTTON_A);
        break;
    case Qt::Key_X:
        m_input->setButton(BUTTON_B);
        break;
    case Qt::Key_Comma:
        m_input->setButton(BUTTON_START);
        break;
    case Qt::Key_Period:
        m_input->setButton(BUTTON_SELECT);
        break;
    case Qt::Key_Up:
        m_input->setButton(BUTTON_UP);
        break;
    case Qt::Key_Down:
        m_input->setButton(BUTTON_DOWN);
        break;
    case Qt::Key_Left:
        m_input->setButton(BUTTON_LEFT);
        break;
    case Qt::Key_Right:
        m_input->setButton(BUTTON_RIGHT);
        break;
    }
}

void nemus::ui::Screen::keyReleaseEvent(QKeyEvent *event)
{
    event->accept();
    switch (event->key())
    {
    case Qt::Key_Z:
        m_input->unsetButton(BUTTON_A);
        break;
    case Qt::Key_X:
        m_input->unsetButton(BUTTON_B);
        break;
    case Qt::Key_Comma:
        m_input->unsetButton(BUTTON_START);
        break;
    case Qt::Key_Period:
        m_input->unsetButton(BUTTON_SELECT);
        break;
    case Qt::Key_Up:
        m_input->unsetButton(BUTTON_UP);
        break;
    case Qt::Key_Down:
        m_input->unsetButton(BUTTON_DOWN);
        break;
    case Qt::Key_Left:
        m_input->unsetButton(BUTTON_LEFT);
        break;
    case Qt::Key_Right:
        m_input->unsetButton(BUTTON_RIGHT);
        break;
    }
}

void nemus::ui::Screen::updateFPS()
{
    std::chrono::system_clock::time_point newTime = std::chrono::system_clock::now();
    std::chrono::duration<double> deltaTime = newTime - m_oldTime;

    std::string title = "NEmuS - FPS: ";
    title += std::to_string(static_cast<int>((1 / deltaTime.count())));
    setWindowTitle(title.c_str());

    m_oldTime = newTime;
}

void nemus::ui::Screen::create_menu()
{
    m_loadRomAction = new QAction(tr("&Load ROM..."), this);
    connect(m_loadRomAction, &QAction::triggered, this, &Screen::openRom);

    m_settingsAction = new QAction(tr("Settings..."), this);
    connect(m_settingsAction, &QAction::triggered, this, &Screen::openSettings);

    m_exitAction = new QAction(tr("&Exit"), this);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_loadRomAction);
    m_fileMenu->addAction(m_settingsAction);
    m_fileMenu->addAction(m_exitAction);
}

void nemus::ui::Screen::closeEvent(QCloseEvent *event)
{
    event->accept();
    m_quit = true;
}

#ifndef QT_NO_CONTEXTMENU
void nemus::ui::Screen::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_loadRomAction);
    menu.addAction(m_settingsAction);
    menu.addAction(m_exitAction);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void nemus::ui::Screen::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    QImage image((unsigned char *)m_ppu->getPixels(), SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32);

    switch (m_state->getScale())
    {
    case SCALE_1X:
        painter.drawPixmap(0, SCREEN_OFFSET, QPixmap::fromImage(image));
        break;
    case SCALE_2X:
        painter.drawPixmap(0, SCREEN_OFFSET, QPixmap::fromImage(image).scaled(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2));
        break;
    case SCALE_3X:
        painter.drawPixmap(0, SCREEN_OFFSET, QPixmap::fromImage(image).scaled(SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3));
        break;
    case SCALE_4X:
        painter.drawPixmap(0, SCREEN_OFFSET, QPixmap::fromImage(image).scaled(SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4));
        break;
    }
}

// TODO: Make loading files and archives helper functions.
void nemus::ui::Screen::openRom()
{
    static constexpr const char *NesFileExtension = ".nes";
    static constexpr const char *ZipFileExtension = ".zip";

    auto filename = QFileDialog::getOpenFileName(
        this, tr("Open Rom"), "", tr("ROM Files (*.nes);;Archive Files (*.zip);;All Files (*)"));

    if (filename.length() > 0)
    {
        std::vector<char> fileContents;
        if (filename.endsWith(NesFileExtension))
        {
            std::ifstream file(filename.toStdString(), std::ios::ate | std::ios::binary);
            if (file.is_open())
            {
                auto size = file.tellg();
                file.seekg(file.beg);

                fileContents.resize(size);
                file.read(fileContents.data(), size);
            }
            else
            {
                // TODO: Make a message box.
                return;
            }
        }
        else if (filename.endsWith(ZipFileExtension))
        {
            QuaZip file(filename);
            if (!file.open(QuaZip::Mode::mdUnzip))
            {
                fmt::print(fmt::fg(fmt::color::tomato), "Unable to open archive: {}\n", filename.toStdString());
                return;
            }

            auto fileList = file.getFileNameList();
            auto romFilename = std::find_if(fileList.begin(), fileList.end(), [](const QString &f)
                                            { return f.endsWith(NesFileExtension); });
            if (romFilename != fileList.end())
            {
                file.setCurrentFile(*romFilename);
                QuaZipFile romFile(&file);
                if (!romFile.open(QIODeviceBase::ReadOnly))
                {
                    fmt::print(fmt::fg(fmt::color::tomato), "Unable to open file in archive: {}\n", romFilename->toStdString());
                    return;
                }
                auto romContents = romFile.readAll();
                fileContents.resize(romContents.size());
                memcpy(fileContents.data(), romContents.data(), fileContents.size());
            }
            else
            {
                fmt::print(fmt::fg(fmt::color::tomato), "Cannot find '.nes' file in archive: {}\n", filename.toStdString());
                return;
            }
        }
        else
        {
            // TODO: Better logger.
            fmt::print(fmt::fg(fmt::color::tomato),
                       "Invalid file extension: {}\n"
                       "Sorry a better file detection system is not a priority :(\n",
                       filename.toStdString());
        }

        m_nes->loadGame(fileContents);
    }
}

void nemus::ui::Screen::openSettings()
{
    Settings dialog(this, m_state);

    dialog.exec();

    applySettings();
}

void nemus::ui::Screen::applySettings()
{
    switch (m_state->getScale())
    {
    case SCALE_1X:
        resize(SCREEN_WIDTH, SCREEN_HEIGHT + SCREEN_OFFSET);
        break;
    case SCALE_2X:
        resize(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2 + SCREEN_OFFSET);
        break;
    case SCALE_3X:
        resize(SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3 + SCREEN_OFFSET);
        break;
    case SCALE_4X:
        resize(SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4 + SCREEN_OFFSET);
        break;
    }

    this->update();
}