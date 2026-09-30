.PHONY: all build clean web-clean prod-clean asan-clean clean-all rebuild install asan-build

all: build

build:
	cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake
	cmake --build build

# Separate build-asan/ directory, not a flag on the regular build/ — mixing
# sanitizer and non-sanitizer object files under one CMakeCache.txt risks the
# same stale-cached-variable problem documented elsewhere in this project
# (SDL2_DIR, CMAKE_OSX_SYSROOT); a fresh directory sidesteps it entirely and
# lets both builds coexist. Run the app or `ctest --test-dir build-asan` from
# here to get ASan/UBSan's crash-on-violation behavior instead of silent
# memory corruption.
asan-build:
	cmake -S . -B build-asan -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake -DGBC_ENABLE_ASAN=ON
	cmake --build build-asan

# Full removal, not `cmake --build --target clean` — that only deletes
# compiled objects and leaves CMakeCache.txt/CMakeFiles/ (the CLion
# index-lockup culprit) sitting there. rm -rf actually makes it go away.
clean:
	rm -rf build cmake-build-debug

web-clean:
	rm -rf build-web

asan-clean:
	rm -rf build-asan

# dist/ (cmake --install output) and build/package/ (cpack staging) hold
# every embedded dylib from the .app bundle fixup — large, binary-heavy,
# and not something a source indexer should ever look at.
prod-clean:
	rm -rf dist/

clean-all: clean web-clean prod-clean asan-clean

rebuild: clean build

install:
	cmake --install build --prefix "$(CURDIR)/dist"
