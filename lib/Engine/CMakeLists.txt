project(Engine)
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic")

# Configure CMake
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_CXX_STANDARD 11)

# CXX Flags
if(MSVC)
	set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /experimental:external /external:anglebrackets /external:W0 /Wall /GR /EHsc")
	if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /MTd")
	endif()
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic")
endif()

# Variable definitions
set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# SDL Dependency
set(SDL_SHARED ON CACHE BOOL "Build a shared version of the library")
set(SDL_STATIC OFF CACHE BOOL "Build a static version of the library")
add_subdirectory("${DEPENDENCIES_DIR}/SDL2" "${CMAKE_BINARY_DIR}/SDL2")
get_property(SOMETHING VARIABLE PROPERTY EXTRA_LIBS)
set(SDL_INCLUDE_DIRS "${SDL2_BINARY_DIR}/include" "${SDL2_SOURCE_DIR}/include")
set(SDL_LIBS SDL2 SDL2main)

# Code Generation
file(GLOB_RECURSE ENGINE_IN_FILES "${SOURCE_DIR}/*.in")
foreach(IN_FILE IN ITEMS ${ENGINE_IN_FILES})
	string(REPLACE ".in" "" FINAL "${IN_FILE}")
	message("Generating ${FINAL}")
	configure_file("${IN_FILE}" "${FINAL}" @ONLY)
endforeach()

# Source gathering
file(GLOB_RECURSE SOURCES 
    "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.c"
    "${SOURCE_DIR}/*.hpp" "${SOURCE_DIR}/*.h")

# Make library
add_library(Engine_Static SHARED ${SOURCES})

# Include directotories
target_include_directories(Engine_Static 
    PUBLIC
    "${SOURCE_DIR}" 
    "${SDL_INCLUDE_DIRS}"
)

# Link dependencies
target_link_libraries(Engine_Static PUBLIC ${SDL_LIBS})

# Source group generation for IDEs
foreach(SOURCE IN ITEMS ${SOURCES})
    get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
    file(RELATIVE_PATH SOURCE_PATH_REL "${SOURCE_DIR}/.." "${SOURCE_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH_REL}")
    source_group("${GROUP_PATH}" FILES "${SOURCE}")
endforeach()