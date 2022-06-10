#include <QApplication>
#include <memory>

#include "Core/NES.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    auto nes = std::make_unique<nemus::NES>();
    nes->run();

    return 0;
}
