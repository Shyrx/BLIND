find_program(ARDUINO-CLI arduino-cli)

if(ARDUINO-CLI-NOTFOUND)
  message("Missing program arduino-cli")
else()
  add_custom_target(arduino
    COMMAND arduino-cli compile --fqbn arduino:avr:uno
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    SOURCES "car.ino")
  add_custom_target(flash
    COMMAND arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS arduino voir)
endif()
