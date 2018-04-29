#include "Core/NES.h"

int main(int argc, char** argv) {
    auto* nes = new nemus::NES();

    nes->run();

    delete nes;

    return 0;
}