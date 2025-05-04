cmake_minimum_required(VERSION 3.10.0)
project(Chip8 VERSION 0.1.0 LANGUAGES C CXX)

# Point to the source files inside the 'src' directory
add_executable(Chip8 src/main.cpp src/Chip8.cpp) 

# Add 'src' directory to include paths so main.cpp can find Chip8.h
target_include_directories(Chip8 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(SDL3 REQUIRED CONFIG)
# Link SDL3
target_link_libraries(Chip8 PRIVATE SDL3::SDL3)