#!/usr/bin/env bash
# Build the emulator to WebAssembly via Emscripten.
# Output lands in build-web/ as gbc.html / gbc.js / gbc.wasm / gbc.data.
set -euo pipefail

cd "$(dirname "$0")"

if [ -z "${EMSDK:-}" ]; then
  source "$HOME/emsdk/emsdk_env.sh" > /dev/null
fi

mkdir -p build-web

emcc \
  -std=c++20 \
  -O2 \
  -I../include \
  -s USE_SDL=2 \
  -s INITIAL_MEMORY=67108864 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s EXIT_RUNTIME=0 \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
  --preload-file ../p-docs/cartridges/tetris-dx.gbc@/rom.gbc \
  --shell-file ../src/web-src/web_shell.html \
  ../src/apu/GBC_APU.cpp \
  ../src/bus/GBC_BUS.cpp \
  ../src/cart/GBC_CART.cpp \
  ../src/cpu/GBC_CPU.cpp \
  ../src/display/GBC_DISPLAY.cpp \
  ../src/display/GBC_PPU.cpp \
  ../src/timer/GBC_TIMER.cpp \
  ../src/web-src/main_web.cpp \
  -o build-web/gbc.html

echo "Built: build-web/gbc.{html,js,wasm,data}"