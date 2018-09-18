project(Game)
cmake_minimum_required(VERSION 2.8.12)

# Configure CMake
set(CMAKE_CXX_STANDARD 14)

# Variable definitions
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Dependencies
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../lib/Engine" "${CMAKE_BINARY_DIR}/Engine")

# Code Generation
file(GLOB_RECURSE GAME_IN_FILES "${SOURCE_DIR}/*.in")
foreach(IN_FILE IN ITEMS ${GAME_IN_FILES})
	string(REPLACE ".in" "" FINAL "${IN_FILE}")
	message("Generating ${FINAL}")
	configure_file("${IN_FILE}" "${FINAL}" @ONLY)
endforeach()

# Source gathering
file(GLOB_RECURSE SOURCES 
    "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.c"
    "${SOURCE_DIR}/*.hpp" "${SOURCE_DIR}/*.h")

# Make library
add_library(Game_Static STATIC ${SOURCES})

# Include directotories
target_include_directories(Game_Static PUBLIC "${SOURCE_DIR}")

# Link dependencies
target_link_libraries(Game_Static PUBLIC Engine_Static)

# Source group generation for IDEs
foreach(SOURCE IN ITEMS ${SOURCES})
    get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
    file(RELATIVE_PATH SOURCE_PATH_REL "${SOURCE_DIR}" "${SOURCE_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH_REL}")
    source_group("${GROUP_PATH}" FILES "${SOURCE}")
endforeach()