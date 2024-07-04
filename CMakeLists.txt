# CMakeLists.txt
cmake_minimum_required (VERSION 3.8.2)
project (gpmf-parser)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Source files
file(GLOB LIB_SOURCES "../GPMF_parser.c" "../GPMF_utils.c")
file(GLOB DEMO_SOURCES "GPMF_demo.c" "GPMF_print.c" "GPMF_mp4reader.c")
file(GLOB CSV_WRITER_SOURCES "csv_writer.c")

# Include directories
include_directories("../")

# Library
add_library(GPMF_PARSER_LIB ${LIB_SOURCES})
set_target_properties(GPMF_PARSER_LIB PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

# Executable for GPMF_demo
add_executable(gpmfdemo ${DEMO_SOURCES})
target_link_libraries(gpmfdemo GPMF_PARSER_LIB)

# Executable for csv_writer
add_executable(csv_writer ${CSV_WRITER_SOURCES} ${DEMO_SOURCES})
target_link_libraries(csv_writer GPMF_PARSER_LIB)

# Install targets
install(TARGETS gpmfdemo csv_writer DESTINATION "bin")
install(TARGETS GPMF_PARSER_LIB DESTINATION "lib")

# Install headers
install(FILES "../GPMF_parser.h" "../GPMF_common.h" "../GPMF_utils.h" DESTINATION "include/gpmf-parser")
install(FILES "GPMF_mp4reader.h" DESTINATION "include/gpmf-parser/demo")

# pkg-config file
set(PC_LINK_FLAGS "-l${PROJECT_NAME}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "lib/pkgconfig")