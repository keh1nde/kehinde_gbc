cmake_minimum_required(VERSION 4.1)
project(kehindes_gameboycolor_emulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(kehindes_gameboycolor_emulator main.cpp
        GBC_CPU.h
        GBC_CPU.cpp
        GBC_BUS.h
        GBC_BUS.cpp
        GBC_CART.h
        GBC_CART.cpp
        GBC_PPU.h
        GBC_PPU.cpp)
