set(NAME engine)
add_library(${NAME} STATIC)

target_compile_features(${NAME} PUBLIC c_std_11)
target_compile_definitions(${NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${NAME} PUBLIC -Wall)
endif()

set(THIRD_PARTY_CJSON_DIR "${CMAKE_SOURCE_DIR}/third_party/cJSON")

target_sources(${NAME} PRIVATE
    "${THIRD_PARTY_CJSON_DIR}/cJSON.c"
    "${THIRD_PARTY_CJSON_DIR}/cJSON.h"

    "src/engine.h"
    "src/engine_common.c"
    "src/engine_common.h"
    "src/engine_settings.h"

    "src/core/asset_manager.c"
    "src/core/asset_manager.h"
    "src/core/audio_system.c"
    "src/core/audio_system.h"
    "src/core/camera.c"
    "src/core/camera.h"
    "src/core/components.c"
    "src/core/components.h"
    "src/core/gizmos_system.c"
    "src/core/gizmos_system.h"
    "src/core/graphics_system.c"
    "src/core/graphics_system.h"
    "src/core/particle_system.c"
    "src/core/particle_system.h"
    "src/core/sprite_sheet.c"
    "src/core/sprite_sheet.h"
    "src/core/timer.c"
    "src/core/timer.h"
    "src/core/viewport.h"
    "src/core/viewport.c"

    "src/core/anim/basic_anim_state.h"
    "src/core/anim/basic_anim_state.c"
    "src/core/anim/easing_fct.c"
    "src/core/anim/easing_fct.h"
    "src/core/anim/anim_common.h"
    "src/core/anim/sprite_anim.c"
    "src/core/anim/sprite_anim.h"
    "src/core/anim/sprite_anim_state.c"
    "src/core/anim/sprite_anim_state.h"
    "src/core/anim/sprite_anim_manager.h"
    "src/core/anim/sprite_anim_manager.c"

    "src/ui/ui_button.c"
    "src/ui/ui_button.h"
    "src/ui/ui_fill_rect.c"
    "src/ui/ui_fill_rect.h"
    "src/ui/ui_focus_manager.c"
    "src/ui/ui_focus_manager.h"
    "src/ui/ui_grid_layout.c"
    "src/ui/ui_grid_layout.h"
    "src/ui/ui_image.c"
    "src/ui/ui_image.h"
    "src/ui/ui_input.c"
    "src/ui/ui_input.h"
    "src/ui/ui_label.c"
    "src/ui/ui_label.h"
    "src/ui/ui_list.c"
    "src/ui/ui_list.h"
    "src/ui/ui_object.h"
    "src/ui/ui_object.c"
    "src/ui/ui_selectable.c"
    "src/ui/ui_selectable.h"
    "src/ui/ui_system.h"
    "src/ui/ui_system.c"
    "src/ui/ui_utils.c"
    "src/ui/ui_utils.h"

    "src/utils/axis_data.c"
    "src/utils/axis_data.h"
    "src/utils/colors.c"
    "src/utils/colors.h"
    "src/utils/utils.c"
    "src/utils/utils.h"
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
)

#-------------------------------------------------------------------------------
# Other include directories

target_include_directories(
    ${NAME} PUBLIC
    "src"
    "${THIRD_PARTY_CJSON_DIR}"
)
