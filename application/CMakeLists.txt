set(NAME application)

if (WIN32 AND BUILD_GUI)
    add_executable(${NAME} WIN32)
else()
    add_executable(${NAME})
endif()

target_compile_features(${NAME} PUBLIC c_std_11)
target_compile_definitions(${NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${NAME} PUBLIC -Wall)
endif()

target_sources(${NAME} PRIVATE
    "src/main.c"
    "src/settings.h"
    "src/common/common.h"
    "src/common/ui_style.h"
    "src/common/ui_style.c"
    "src/common/assets.c"
    "src/common/assets.h"

    "src/game/game_config.c"
    "src/game/game_config.h"
    "src/game/input.c"
    "src/game/input.h"

    "src/game/core/game_context.c"
    "src/game/core/game_context.h"
    "src/game/core/game_engine.c"
    "src/game/core/game_engine.h"
    
    
    "src/game/core/scene_manager.c"
    "src/game/core/scene_manager.h"

    
    "src/game/scene/title_scene.c"
    "src/game/scene/title_scene.h"
    
    
    
    "src/game/ui/title_main_menu.c"
    "src/game/ui/title_main_menu.h"
    "src/game/ui/title_settings_page.c"
    "src/game/ui/title_settings_page.h"
    "src/game/ui/title_ui_manager.c"
    "src/game/ui/title_ui_manager.h"
)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(${NAME} PUBLIC ${MATH_LIBRARY})
endif()

#-------------------------------------------------------------------------------
# Third party libraries

if(MSVC)
    message(STATUS "MSCV DETECTED")
    set(THIRD_PARTY_SDL3_DIR       "${CMAKE_SOURCE_DIR}/third_party/SDL3-3.4.4")
    set(THIRD_PARTY_SDL3_IMAGE_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL3_image-3.4.2")
    set(THIRD_PARTY_SDL3_TTF_DIR   "${CMAKE_SOURCE_DIR}/third_party/SDL3_ttf-3.2.2")
    set(THIRD_PARTY_SDL3_MIXER_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL3_mixer-3.2.0")

    list(APPEND CMAKE_PREFIX_PATH
        "${THIRD_PARTY_SDL3_DIR}/cmake"
        "${THIRD_PARTY_SDL3_IMAGE_DIR}/cmake"
        "${THIRD_PARTY_SDL3_TTF_DIR}/cmake"
        "${THIRD_PARTY_SDL3_MIXER_DIR}/cmake"
    )
endif()

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_mixer REQUIRED)

target_link_libraries(${NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
    engine
)

#-------------------------------------------------------------------------------
# Other include directories

target_include_directories(
    ${NAME} PUBLIC
    "src"
)

#-------------------------------------------------------------------------------
# Copy DLL and files

if(WIN32)
    # Use the imported target file locations so this works with vcpkg, installer, etc.
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:${NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_image::SDL3_image> $<TARGET_FILE_DIR:${NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_ttf::SDL3_ttf> $<TARGET_FILE_DIR:${NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_mixer::SDL3_mixer> $<TARGET_FILE_DIR:${NAME}>
    )
endif()
