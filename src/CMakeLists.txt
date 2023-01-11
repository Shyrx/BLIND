add_subdirectory("process")
add_subdirectory("serial")

add_library(argparser "arg-parser.cc")
find_package(
  Boost 1.74.0
  COMPONENTS program_options
  REQUIRED)
target_include_directories(argparser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(argparser PRIVATE ${Boost_LIBRARIES})
