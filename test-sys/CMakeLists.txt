cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)
find_package(Stb REQUIRED)


add_executable(
        gbc_test
        test_utils.cpp
        test_utils.h
        blargg_test.cc
        mooneye_test.cc
        cgb_acid2_test.cc)

target_link_libraries(
        gbc_test PRIVATE
        gbc_core
        GTest::gtest_main
)

target_include_directories(gbc_test PRIVATE ${Stb_INCLUDE_DIR})
target_compile_definitions(gbc_test PRIVATE GBC_REPO_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/..")

include(GoogleTest)
gtest_discover_tests(gbc_test)