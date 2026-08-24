.PHONY: all build clean web-clean prod-clean clean-all rebuild install

all: build

build:
	cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake
	cmake --build build

# Full removal, not `cmake --build --target clean` — that only deletes
# compiled objects and leaves CMakeCache.txt/CMakeFiles/ (the CLion
# index-lockup culprit) sitting there. rm -rf actually makes it go away.
clean:
	rm -rf build cmake-build-debug

web-clean:
	rm -rf build-web

# dist/ (cmake --install output) and build/package/ (cpack staging) hold
# every embedded dylib from the .app bundle fixup — large, binary-heavy,
# and not something a source indexer should ever look at.
prod-clean:
	rm -rf dist build/package

clean-all: clean web-clean prod-clean

rebuild: clean build

install:
	cd ..
	cmake --install build --prefix "$(CURDIR)/dist"
