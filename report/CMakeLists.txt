add_custom_target(
    report
  COMMAND pandoc -o report.pdf report.md
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
