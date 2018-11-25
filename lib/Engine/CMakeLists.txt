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
if(NOT DEFINED RESOURCES_DIR)
	message(WARNING "RESOURCES_DIR not set! We might not be able to find the resources.")
	set(RESOURCES_DIR "TEST")
else()
	message(STATUS "RESOURCES_DIR -> ${RESOURCES_DIR}")
endif()

# SDL2 Dependency
set(SDL2_PATH "${DEPENDENCIES_DIR}/SDL2")
find_package(SDL2 REQUIRED)
message(STATUS "SDL2 Version -> ${SDL2_VERSION_STRING}")

# SDL2_image Dependency
set(SDL2_IMAGE_PATH "${DEPENDENCIES_DIR}/SDL2_image")
find_package(SDL2_image REQUIRED)
message(STATUS "SDL2_Image Version -> ${SDL2_IMAGE_VERSION_STRING}")

# SDL2_ttf Dependency
set(SDL2_TTF_PATH "${DEPENDENCIES_DIR}/SDL2_ttf")
find_package(SDL2_ttf REQUIRED)
message(STATUS "SDL2_TTF Version -> ${SDL2_TTF_VERSION_STRING}")

# glm Dependency
set(glm_PATH "${DEPENDENCIES_DIR}/glm/glm")
add_subdirectory(${glm_PATH})

# Code Generation
file(GLOB_RECURSE ENGINE_IN_FILES "${SOURCE_DIR}/*.in")
foreach(IN_FILE IN ITEMS ${ENGINE_IN_FILES})
	string(REPLACE ".in" "" FINAL "${IN_FILE}")
	message(STATUS "Generating ${FINAL}")
	configure_file("${IN_FILE}" "${FINAL}" @ONLY)
endforeach()

# Source gathering
file(GLOB_RECURSE SOURCES 
    "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.hpp")

# Make library
add_library(Engine_Static STATIC ${SOURCES})

# Include directotories
target_include_directories(Engine_Static 
	PUBLIC 
	${SOURCE_DIR}
)

target_include_directories(Engine_Static SYSTEM
    PUBLIC
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
	${glm_PATH}/..
)

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

# Copy SDL dll's to executalbr path
if(MSVC)
	# Check current architecture
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARCH "x64")
	else()
		set(ARCH "x86")
	endif()

	# Gather them all
	file(GLOB_RECURSE SDL2_DLLS "${SDL2_PATH}/lib/${ARCH}/*.dll")
	file(GLOB_RECURSE SDL2_IMAGE_PATH "${SDL2_IMAGE_PATH}/lib/${ARCH}/*.dll")
	file(GLOB_RECURSE SDL2_TTF_PATH "${SDL2_TTF_PATH}/lib/${ARCH}/*.dll")
	list(APPEND SDL2_DLLS ${SDL2_IMAGE_PATH})
	list(APPEND SDL2_DLLS ${SDL2_TTF_PATH})

	# Copy
	foreach(DLL_PATH IN ITEMS ${SDL2_DLLS})
		get_filename_component(DLL_NAME ${DLL_PATH} NAME)
		#add_custom_command(TARGET Engine_Static POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_PATH}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${DLL_NAME}")
		message(STATUS "Copy ${DLL_PATH} -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${DLL_NAME}")
	endforeach()
endif()