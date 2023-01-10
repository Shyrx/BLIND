cmake_minimum_required(VERSION 3.18)

project(blind LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wvla")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address")

find_package(OpenCV REQUIRED)
link_libraries($<$<CONFIG:Debug>:-fsanitize=address> ${OpenCV_LIBS})

include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory("src")

add_executable(blind "src/main.cc")

target_link_libraries(cpu PRIVATE common)

target_link_libraries(blind PRIVATE cpu)
