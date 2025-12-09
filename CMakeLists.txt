cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(NAME sdl3_template)
project(${NAME} VERSION 1.0)

option(BUILD_GUI "Build without console window (Windows only)" OFF)

message(STATUS "[INFO] Current directory: " ${CMAKE_SOURCE_DIR})

#-------------------------------------------------------------------------------
# Third party libraries

if(MSVC)
    message(STATUS "MSCV DETECTED")
    set(THIRD_PARTY_SDL3_DIR       "${CMAKE_SOURCE_DIR}/third_party/SDL3-3.2.8")
    set(THIRD_PARTY_SDL3_IMAGE_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL3_image-3.2.4")
    set(THIRD_PARTY_SDL3_TTF_DIR   "${CMAKE_SOURCE_DIR}/third_party/SDL3_ttf-3.1.0")
    set(THIRD_PARTY_CJSON_DIR      "${CMAKE_SOURCE_DIR}/third_party/cJSON")

    list(APPEND CMAKE_PREFIX_PATH
        "${THIRD_PARTY_SDL3_DIR}/cmake"
        "${THIRD_PARTY_SDL3_IMAGE_DIR}/cmake"
        "${THIRD_PARTY_SDL3_TTF_DIR}/cmake"
    )
endif()

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

#-------------------------------------------------------------------------------
# Projects

add_subdirectory(engine)
add_subdirectory(application)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT application)
endif()
