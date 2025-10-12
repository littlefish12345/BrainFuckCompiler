cmake_minimum_required(VERSION 3.15)

project(BrainFuckCompiler)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_STANDARD 11)

aux_source_directory("${PROJECT_SOURCE_DIR}/src" BRAINFUCKCOMPILER_ALL_SOURCE)

add_executable(bfc ${BRAINFUCKCOMPILER_ALL_SOURCE})

target_include_directories(bfc PUBLIC "${PROJECT_SOURCE_DIR}/include")