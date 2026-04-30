# SDL3 Template

[![MIT License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A small SDL3-based 2D engine and demo application.  
This repository provides a reusable game engine (`engine/`) and a sample application (`application/`) demonstrating UI, sprite sheets, input handling, scenes, and resource management.
See [Space Pixels](https://github.com/ArnaudBannier/space-pixels-sdl3.git) for a more complete example of a game built on top of this engine.

## Features

- Minimal 2D engine in C using SDL3, SDL3_image, SDL3_ttf and cJSON.
- UI system with buttons, lists, labels, layouts and anchor-based positioning.
- - Sprite sheet support, including 9-slice scaling and pixel-art rendering (see the [documentation](docs/sprite_sheet_json_fr.md) for details).
- Demo application showcasing scenes, runtime settings and UI composition.
- Clean project layout suitable as a template for new SDL3 projects.
- Built-in asset obfuscation system:
  * Source assets are placed in `assets_dev/`.
  * The build system can automatically generate obfuscated copies.
  * Obfuscated assets are written to `assets/` and can be loaded normally via the `AssetManager`.

## Recommended Build Setup

The recommended way to build the project is:

- Visual Studio 2026 (or newer)
- CMake version 4.1.2 or higher

### Steps

1. Clone the repository
2. Open the folder in Visual Studio 2026. Visual Studio automatically detects CMake projects.
3. Build and run the project from the IDE.

## Project layout

- `engine/` — static library with engine core: renderer, asset manager, UI components, utilities.
- `application/` — demo application (main, scenes, UI pages, asset registration).
- `third_party/` — optional local copies of SDL3 and cJSON used by MSVC builds.
- `assets_dev/` — images, sprite sheets and data used by the demo.

## Third-party libraries used

- [SDL3](https://github.com/libsdl-org/SDL)
- [SDL3_image](https://github.com/libsdl-org/SDL_image)
- [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)
- [cJSON](https://github.com/DaveGamble/cJSON)

## License

### Code

All source code produced for this project is licensed under the MIT License.
See the `LICENSE.md` file for details.

### Assets

Game assets included in this repository are not covered by the MIT license.
They are provided under various third-party licenses.

Detailed information is available in [ASSETS_LICENSES.md](ASSETS_LICENSES.md) file.

## Educational context

This project is provided as part of the curriculum at __ESIEA__.
It is intended for educational purposes and as a starting point for student projects.
The authors make no claim that the basecode represents a complete or production-ready game.
