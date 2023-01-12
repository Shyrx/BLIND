add_library(serial "serial.cc")

target_include_directories(serial PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

find_library(libpigpio pigpio REQUIRED)
target_link_libraries(serial PRIVATE pigpio)
