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
    "src/game/input.c"
    "src/game/input.h"
    "src/game/scene.c"
    "src/game/scene.h"
    "src/game/ui/game_ui_manager.c"
    "src/game/ui/game_ui_manager.h"
    "src/game/ui/game_title_page.c"
    "src/game/ui/game_title_page.h"
    "src/game/ui/game_settings_page.c"
    "src/game/ui/game_settings_page.h"
    "src/game/game_graphics.h"
    "src/game/game_graphics.c"
    "src/game/core/game_core.c"
    "src/game/core/game_core.h"
    "src/game/game_config.c"
    "src/game/game_config.h"
)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(${NAME} PUBLIC ${MATH_LIBRARY})
endif()

#-------------------------------------------------------------------------------
# Third party libraries

target_link_libraries(${NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
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
    )
endif()
