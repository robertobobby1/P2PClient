cmake_minimum_required(VERSION 3.20)
project(P2PClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

file(GLOB MyCPPSources ${CMAKE_CURRENT_SOURCE_DIR}/P2PClient/Source/*.cpp)
add_executable(P2PClient ${MyCPPSources})

target_include_directories(P2PClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/P2PClient/Include)

target_compile_options(P2PClient PRIVATE -O2 -Wall -g -Wno-missing-braces)
target_compile_options(P2PClient PRIVATE -Wall)
target_compile_options(P2PClient PRIVATE -g)
