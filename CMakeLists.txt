cmake_minimum_required(VERSION 2.8)

project(RobotsWar)

include_directories(./include)

set(CMAKE_CXX_COMPILER "clang++-6.0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexceptions -std=c++17 -Wextra -Wno-c++98-compat -Wno-padded -O2")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s  -lfparser -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -lstdc++fs")

file(GLOB_RECURSE CPP_FILES ./src/Blocks/*.cpp ./src/Commands/*.cpp ./src/Rendering/*.cpp ./src/*.cpp)
file(GLOB_RECURSE HPP_FILES ./include/Blocks/*.h ./include/Commands/*.h ./include/Rendering/*.h ./include/*.h)

add_executable(${PROJECT_NAME} ${HPP_FILES} ${CPP_FILES})

target_link_libraries(${PROJECT_NAME} stdc++fs)

