#include "Core/NES.h"
#include <QApplication>

int main(int argc, char** argv) {
    QApplication a(argc, argv);

    auto* nes = new nemus::NES();

    nes->run();

    delete nes;

    return 0;
}
