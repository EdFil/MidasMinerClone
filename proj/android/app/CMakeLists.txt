cmake_minimum_required(VERSION 3.4.1)

# Configure CMake
set (CMAKE_CXX_STANDARD 14)

# Variable definitions
set(ROOT_PROJ_DIR "${CMAKE_SOURCE_DIR}/../../..")
set(RESOURCES_DIR "??")

# CXX Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic")

# Build game lib
add_subdirectory("${ROOT_PROJ_DIR}/src" "${CMAKE_BINARY_DIR}/Game")

# Build main game lib
add_library(main SHARED "src/main/jni/main.cpp")

# Link game libs
target_link_libraries(main PRIVATE Game_Static)
