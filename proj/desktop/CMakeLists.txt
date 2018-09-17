project(Concept)
cmake_minimum_required(VERSION 2.8.12)

# Configure CMake
set (CMAKE_CXX_STANDARD 14)

# Variable definitions
set(ROOT_PROJ_DIR "${CMAKE_SOURCE_DIR}/../..")
set(RESOURCES_DIR "${ROOT_PROJ_DIR}/res")

# CXX Flags
if(MSVC)
	set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /experimental:external /external:anglebrackets /external:W0 /Wall /GR /EHsc")
	if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /MTd")
	endif()
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic")
endif()

# Build game lib
add_subdirectory("${ROOT_PROJ_DIR}/src" "${CMAKE_BINARY_DIR}/Game")

# Create executable
add_executable(Concept "main.cpp")

# Link external libs
target_link_libraries(Concept 
    PRIVATE Game_Static
)