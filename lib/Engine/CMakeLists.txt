project(Engine)
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ansi -pedantic")

# Configure CMake
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 14)

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

# SDL2 Dependency
set(SDL2_PATH "${DEPENDENCIES_DIR}/SDL2")
find_package(SDL2 REQUIRED)
message(STATUS "SDL2_FOUND -> ${SDL2_FOUND}")
message(STATUS "SDL2_LIBRARY -> ${SDL2_LIBRARY}")
message(STATUS "SDL2_INCLUDE_DIR -> ${SDL2_INCLUDE_DIR}")

# SDL2_image Dependency
set(SDL2_IMAGE_PATH "${DEPENDENCIES_DIR}/SDL2_image")
find_package(SDL2_image REQUIRED)
message(STATUS "SDL2_IMAGE_FOUND -> ${SDL2_IMAGE_FOUND}")
message(STATUS "SDL2_IMAGE_LIBRARIES -> ${SDL2_IMAGE_LIBRARIES}")
message(STATUS "SDL2_IMAGE_INCLUDE_DIRS -> ${SDL2_IMAGE_INCLUDE_DIRS}")
message(STATUS "SDL2_IMAGE_VERSION_STRING -> ${SDL2_IMAGE_VERSION_STRING}")

# SDL2_ttf Dependency
set(SDL2_TTF_PATH "${DEPENDENCIES_DIR}/SDL2_ttf")
find_package(SDL2_ttf REQUIRED)
message(STATUS "SDL2_TTF_FOUND -> ${SDL2_TTF_FOUND}")
message(STATUS "SDL2_TTF_LIBRARIES -> ${SDL2_TTF_LIBRARIES}")
message(STATUS "SDL2_TTF_INCLUDE_DIRS -> ${SDL2_TTF_INCLUDE_DIRS}")
message(STATUS "SDL2_TTF_VERSION_STRING -> ${SDL2_TTF_VERSION_STRING}")

# GLM Depencency
set(GLM_TEST_ENABLE OFF)
add_subdirectory("${DEPENDENCIES_DIR}/glm")

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
add_library(Engine_Static STATIC ${SOURCES})

# Include directotories
target_include_directories(Engine_Static 
    PUBLIC
    ${SOURCE_DIR} 
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${glm_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(Engine_Static 
    PUBLIC 
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
    glm_static
)

# Source group generation for IDEs
foreach(SOURCE IN ITEMS ${SOURCES})
    get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
    file(RELATIVE_PATH SOURCE_PATH_REL "${SOURCE_DIR}/.." "${SOURCE_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH_REL}")
    source_group("${GROUP_PATH}" FILES "${SOURCE}")
endforeach()

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()