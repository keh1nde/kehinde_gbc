cmake_minimum_required(VERSION 4.1)
project(kehindes_gameboycolor_emulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(kehindes_gameboycolor_emulator main.cpp
        GameBoyColor.h
        GameBoyColor.cpp)
