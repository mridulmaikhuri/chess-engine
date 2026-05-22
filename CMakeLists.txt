cmake_minimum_required(VERSION 3.16)

project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 REQUIRED)

add_executable(app src/main.cpp)

target_link_libraries(app PRIVATE SDL3::SDL3)