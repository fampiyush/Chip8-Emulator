cmake_minimum_required(VERSION 3.10.0)
project(Chip8 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Chip8 main.cpp Chip8.cpp)
find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(Chip8 PRIVATE SDL3::SDL3)