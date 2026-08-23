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
find_package(SDL2_image REQUIRED)

add_executable(kehindes_gameboycolor_emulator MACOSX_BUNDLE ../main.cpp
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
target_link_libraries(kehindes_gameboycolor_emulator PRIVATE ${SDL2_LIBRARIES} SDL2_image::SDL2_image)

# --- macOS .app bundle packaging ---
# MACOSX_BUNDLE (above) makes `cmake --build` produce a .app instead of a bare
# binary. Everything below is install-time (`cmake --install`), not build-time:
# it copies the bundle's dependent dylibs into Contents/Frameworks and rewrites
# their load paths to be relative to the bundle, so a downloaded release
# doesn't require Homebrew SDL2/SDL2_image to be installed on the machine
# running it (see otool -L on a plain build — it hardcodes /opt/homebrew paths).
set_target_properties(kehindes_gameboycolor_emulator PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    MACOSX_BUNDLE_BUNDLE_NAME "kehindes-gbc"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.kehindeadeoso.gbc"
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
)

if(APPLE)
    # Copy assets/ into the bundle's Contents/Resources/ after every build,
    # not just at install time — a plain `cmake --build` (which is all CLion
    # runs by default) never executes install(), so without this the bundle
    # sitting in cmake-build-debug/ would have an empty Resources/ and
    # IMG_Load would fail with "No such file or directory" even though the
    # path logic itself is correct.
    add_custom_command(TARGET kehindes_gameboycolor_emulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/../assets
            "$<TARGET_BUNDLE_CONTENT_DIR:kehindes_gameboycolor_emulator>/Resources/assets"
        COMMENT "Copying assets/ into the built bundle's Contents/Resources/"
    )
endif()

install(TARGETS kehindes_gameboycolor_emulator BUNDLE DESTINATION .)

# assets/ -> Contents/Resources/assets/ in the *installed* copy (a separate
# tree from the one POST_BUILD above populates in the build directory).
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../assets
        DESTINATION kehindes_gameboycolor_emulator.app/Contents/Resources)

if(APPLE)
    # Extra search dirs for transitive @rpath-relative deps (e.g. SDL2_image's
    # own webp/jxl dependencies reference *their* deps by @rpath, which
    # fixup_bundle can't resolve without being told where to look).
    #
    # fixup_bundle rewrites load commands with install_name_tool, which
    # invalidates the original (often Homebrew-provided) code signatures on
    # every dylib it touches. On Apple Silicon the kernel enforces code
    # signing at load time regardless of Gatekeeper, so a bundle with
    # invalidated signatures gets SIGKILLed on launch, not just Gatekeeper-
    # warned. Re-sign everything ad-hoc (-s -) afterward so it actually runs;
    # this does NOT solve the separate "unidentified developer" Gatekeeper
    # warning a downloaded, non-notarized release will still show.
    # A single `--deep` sign on the outer bundle, done last, is the only
    # combination that produces a valid signature here — pre-signing the
    # Frameworks/*.dylib files individually first (a natural-seeming first
    # attempt) breaks the bundle's resource seal and produces a signature
    # that verifies as invalid ("code or signature have been modified").
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/kehindes_gameboycolor_emulator.app\" \"\" \"/opt/homebrew/lib\")
        execute_process(
            COMMAND codesign --force --deep --sign - \"\${CMAKE_INSTALL_PREFIX}/kehindes_gameboycolor_emulator.app\"
            RESULT_VARIABLE _gbc_codesign_result
        )
        if(NOT _gbc_codesign_result EQUAL 0)
            message(FATAL_ERROR \"codesign failed (exit \${_gbc_codesign_result}) — bundle will not launch\")
        endif()
    ")
endif()

set(CPACK_PACKAGE_NAME "kehindes-gbc")
set(CPACK_PACKAGE_VERSION "${MACOSX_BUNDLE_SHORT_VERSION_STRING}")
set(CPACK_GENERATOR "DragNDrop")
include(CPack)
