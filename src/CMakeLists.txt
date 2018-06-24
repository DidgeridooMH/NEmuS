cmake_minimum_required(VERSION 3.8)
project(NEmuS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ --disable-stdio-redirect")

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib/x64)

set(SOURCE_FILES main.cpp Core/NES.cpp Core/NES.h Core/include/ComponentHelper.h Core/CPU.cpp Core/CPU.h Core/Memory.cpp Core/Memory.h Debug/Logger.cpp Debug/Logger.h Core/PPU.cpp Core/PPU.h UI/Screen.cpp UI/Screen.h Core/Mappers/Mapper.h Core/Mappers/NROM.h Core/Mappers/NROM.cpp)
add_executable(NEmuS ${SOURCE_FILES})

target_link_libraries(NEmuS SDL2main SDL2)