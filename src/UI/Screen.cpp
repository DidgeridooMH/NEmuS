#include <chrono>
#include <QApplication>
#include <QAction>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMenuBar>
#include <QPainter>
#include <QVBoxLayout>
#include "Screen.h"
#include "../Core/NES.h"

nemus::ui::Screen::Screen(core::PPU *ppu, NES* nes, QWidget* parent) : QMainWindow(parent) {
    m_ppu = ppu;
    m_nes = nes;

    QWidget* widget = new QWidget;
    setCentralWidget(widget);

    QWidget* topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    create_menu();

    std::string title = "NEmuS Alpha";
    setWindowTitle(QString::fromStdString(title));
    resize(SCREEN_WIDTH, SCREEN_HEIGHT + SCREEN_OFFSET);
    show();

    m_oldTime = std::chrono::system_clock::now();
}

void nemus::ui::Screen::updateWindow() {
    QApplication::processEvents();
    update();
}

void nemus::ui::Screen::updateFPS() {
    std::chrono::system_clock::time_point newTime = std::chrono::system_clock::now();
    std::chrono::duration<double> deltaTime = newTime - m_oldTime;

    std::string title = "NEmuS - FPS: ";
    title += std::to_string(deltaTime.count());
    setWindowTitle(title.c_str());

    m_oldTime = newTime;
}

void nemus::ui::Screen::create_menu() {
    m_loadRomAction = new QAction(tr("&Load ROM... (Fast)"), this);
    connect(m_loadRomAction, &QAction::triggered, this, &Screen::openRom);

    m_exitAction = new QAction(tr("&Exit"), this);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_loadRomAction);
    m_fileMenu->addAction(m_exitAction);
}

void nemus::ui::Screen::closeEvent(QCloseEvent *event) {
    event->accept();
    m_quit = true;
}

#ifndef QT_NO_CONTEXTMENU
void nemus::ui::Screen::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    menu.addAction(m_loadRomAction);
    menu.addAction(m_exitAction);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void nemus::ui::Screen::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    QImage image((unsigned char*)m_ppu->getPixels(), SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32);

    painter.drawPixmap(0, SCREEN_OFFSET, QPixmap::fromImage(image));
}

void nemus::ui::Screen::openRom() {
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Rom"), "", tr("ROM Files (*.nes)"));
    m_nes->loadGame(file_name.toStdString());
}