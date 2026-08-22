cmake_minimum_required(VERSION 4.1)

# Default CLT SDK on this machine outpaces AppleClang's libc++ builtins
# (__builtin_ctzg/__builtin_clzg); pin to an older SDK if present.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" AND NOT DEFINED CMAKE_OSX_SYSROOT)
    set(_gbc_fallback_sdk "/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk")
    if(EXISTS "${_gbc_fallback_sdk}")
        set(CMAKE_OSX_SYSROOT "${_gbc_fallback_sdk}" CACHE PATH "SDK root")
    endif()
    unset(_gbc_fallback_sdk)
endif()

project(kehindes_gameboycolor_emulator)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(kehindes_gameboycolor_emulator ../main.cpp
        ../include/GBC_CPU.h
        ../src/cpu/GBC_CPU.cpp
        ../include/GBC_BUS.h
        ../src/bus/GBC_BUS.cpp
        ../include/GBC_CART.h
        ../src/cart/GBC_CART.cpp
        ../include/GBC_PPU.h
        ../src/display/GBC_PPU.cpp
        ../include/GBC_Types.h
        ../include/test-shims/TestBus.h
        ../src/test-shims/TestBus.cpp
        ../include/test-shims/TestCart.h
        ../src/test-shims/TestCart.cpp
        ../include/GBC_TIMER.h
        ../src/timer/GBC_TIMER.cpp
        ../include/GBC_DISPLAY.h
        ../src/display/GBC_DISPLAY.cpp
        ../include/GBC_APU.h
        ../src/apu/GBC_APU.cpp)

target_include_directories(kehindes_gameboycolor_emulator PRIVATE ${SDL2_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(kehindes_gameboycolor_emulator PRIVATE ${SDL2_LIBRARIES})
