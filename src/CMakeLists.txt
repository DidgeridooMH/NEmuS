cmake_minimum_required(VERSION 3.8)
project(NEmuS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ --disable-stdio-redirect")
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib/x64)

set(SOURCE 
    main.cpp
    Core/NES.cpp
    Core/CPU.cpp
    Core/Memory.cpp
    Debug/Logger.cpp
    Core/PPU.cpp
    UI/Screen.cpp
    Core/Mappers/NROM.cpp
    Core/Mappers/MMC1.cpp
    Core/Input.cpp
    )

set(HEADERS
    Core/NES.h
    Core/include/ComponentHelper.h
    Core/CPU.h
    Core/Memory.h
    Debug/Logger.h
    Core/PPU.h
    UI/Screen.h
    Core/Mappers/Mapper.h
    Core/Mappers/NROM.h
    Core/Mappers/MMC1.h
    Core/Input.h
    )

add_executable(NEmuS ${SOURCE} ${HEADERS})

target_link_libraries(NEmuS Qt5::Core Qt5::Widgets)