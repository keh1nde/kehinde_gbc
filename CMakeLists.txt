cmake_minimum_required(VERSION 4.1)
project(kehindes_gameboycolor_emulator)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(kehindes_gameboycolor_emulator main.cpp
        GBC_CPU.h
        GBC_CPU.cpp
        GBC_BUS.h
        GBC_BUS.cpp
        GBC_CART.h
        GBC_CART.cpp
        GBC_PPU.h
        GBC_PPU.cpp
        GBC_Types.h
        TestBus.h
        TestBus.cpp
        TestCart.h
        TestCart.cpp
        GBC_TIMER.h
        GBC_TIMER.cpp
        GBC_DISPLAY.h
        GBC_DISPLAY.cpp)
        GBC_DISPLAY.cpp
        GBC_APU.h
        GBC_APU.cpp)

target_include_directories(kehindes_gameboycolor_emulator PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(kehindes_gameboycolor_emulator PRIVATE ${SDL2_LIBRARIES})
