#include <QApplication>
#include "Core/NES.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);

    nemus::NES* nes = new nemus::NES();

    nes->run();

    delete nes;

    return 0;
}
