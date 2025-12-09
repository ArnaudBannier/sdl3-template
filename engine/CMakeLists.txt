set(NAME engine)
add_library(${NAME} STATIC)

target_compile_features(${NAME} PUBLIC c_std_11)
target_compile_definitions(${NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${NAME} PUBLIC -Wall)
endif()

target_sources(${NAME} PRIVATE
    "src/core/asset_manager.c"
    "src/core/asset_manager.h"
    "src/core/camera.c"
    "src/core/camera.h"
    "src/core/renderer.c"
    "src/core/renderer.h"
    "src/core/sprite_anim.c"
    "src/core/sprite_anim.h"
    "src/core/sprite_sheet.c"
    "src/core/sprite_sheet.h"
    "src/core/timer.c"
    "src/core/timer.h"
    "src/ui/ui_button.c"
    "src/ui/ui_button.h"
    "src/ui/ui_canvas.c"
    "src/ui/ui_canvas.h"
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
    "src/ui/ui_utils.c"
    "src/ui/ui_utils.h"
    "src/utils/colors.c"
    "src/utils/colors.h"
    "src/utils/utils.c"
    "src/utils/utils.h"
    "src/game_engine.h"
    "src/game_engine_common.c"
    "src/game_engine_common.h"
    "src/game_engine_settings.h"
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
)

#-------------------------------------------------------------------------------
# Other include directories

target_include_directories(
    ${NAME} PUBLIC
    "src"
    "${THIRD_PARTY_CJSON_DIR}"
)

#-------------------------------------------------------------------------------
# cJSON files

target_sources("${NAME}" PRIVATE
    "${THIRD_PARTY_CJSON_DIR}/cJSON.c"
    "${THIRD_PARTY_CJSON_DIR}/cJSON.h"
)
