/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "engine_common.h"

extern EngineCommon g_engine = { 0 };

static void Game_createStoragePath(const GameInitParams* params)
{
    const char* basePath = SDL_GetBasePath();
    const int maxPathLen = 1024;

    char* base = (char*)calloc(maxPathLen, sizeof(char));
    ASSERT_NEW(base);
    SDL_strlcat(base, basePath, maxPathLen);

    char* assets = (char*)calloc(maxPathLen, sizeof(char));
    ASSERT_NEW(assets);
    SDL_strlcat(assets, basePath, maxPathLen);
    SDL_strlcat(assets, params->assetsPath, maxPathLen);

    char* projectRoot = (char*)calloc(maxPathLen, sizeof(char));
    ASSERT_NEW(projectRoot);
    SDL_strlcat(projectRoot, basePath, maxPathLen);
    SDL_strlcat(projectRoot, params->projectRootPath, maxPathLen);

    g_engine.paths.base = base;
    g_engine.paths.assets = assets;
    g_engine.paths.projectRoot = projectRoot;
}

static void Game_destroyStoragePath()
{
    free(g_engine.paths.base);
    free(g_engine.paths.assets);
    free(g_engine.paths.projectRoot);

    g_engine.paths.base = NULL;
    g_engine.paths.assets = NULL;
    g_engine.paths.projectRoot = NULL;
}

SDL_AppResult Game_init(const GameInitParams* params)
{
    // Initialise la SDL
    if (!SDL_Init(params->sdlFlags))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_Init %s\n", SDL_GetError());
        assert(false);
        return SDL_APP_FAILURE;
    }

    // Initialise la SDL TTF
    if (!TTF_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "TTF_Init %s\n", SDL_GetError());
        assert(false);
        return SDL_APP_FAILURE;
    }

    if (!MIX_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "MIX_Init %s\n", SDL_GetError());
        assert(false);
        return SDL_APP_FAILURE;
    }

    // Crée le temps global du jeu
    g_engine.time = Timer_create();
    ASSERT_NEW(g_engine.time);

    // Initialise le chemin de stockage des ressources
    Game_createStoragePath(params);

    g_engine.sizes.defaultRenderWidth = 1920.f;
    g_engine.sizes.mainRenderScale = 1.0f;
    g_engine.sizes.viewportSize = Vec2_set(640.f, 360.f);
    g_engine.sizes.uiSize = Vec2_set(640.f, 360.f);
    g_engine.sizes.uiPixelsPerUnit = Vec2_set(1.f, 1.f);

    OpenColors_init(&g_colors);

    return SDL_APP_CONTINUE;
}

SDL_AppResult Game_createWindow(int width, int height, const char* title, Uint32 flags)
{
    assert(g_engine.sdl.window == NULL && "The window is already created");

    g_engine.sdl.window = SDL_CreateWindow(title, width, height, flags);
    if (!g_engine.sdl.window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Create window %s\n", SDL_GetError());
        assert(false);
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Game_createMixer()
{
    SDL_AudioDeviceID audioDeviceID = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
    g_engine.sdl.mixer = MIX_CreateMixerDevice(audioDeviceID, NULL);
    if (!g_engine.sdl.mixer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Create mixer device %s\n", SDL_GetError());
        assert(false);
        abort();
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Game_createRenderer()
{
    assert(g_engine.sdl.renderer == NULL && "The renderer is already created");
    assert(g_engine.sdl.window);

    SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Available rendering engines:\n");
    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "- \"%s\"\n", SDL_GetRenderDriver(i));
    }

    //g_renderer = SDL_CreateRenderer(g_window, "direct3d11");
    //g_renderer = SDL_CreateRenderer(g_window, "direct3d12");
    //g_renderer = SDL_CreateRenderer(g_window, "opengl");
    //g_renderer = SDL_CreateRenderer(g_window, "opengl");
    g_engine.sdl.renderer = SDL_CreateRenderer(g_engine.sdl.window, NULL);

    if (!g_engine.sdl.renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Create renderer %s\n", SDL_GetError());
        assert(false);
        return SDL_APP_FAILURE;
    }

    bool success = true;
    success = SDL_SetRenderDrawBlendMode(g_engine.sdl.renderer, SDL_BLENDMODE_BLEND);
    assert(success);

    if (!SDL_SetRenderVSync(g_engine.sdl.renderer, 1))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Render VSync %s\n", SDL_GetError());
    }

    g_engine.sdl.textEngine = TTF_CreateRendererTextEngine(g_engine.sdl.renderer);
    if (!g_engine.sdl.textEngine)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Create ttfText engine %s\n", SDL_GetError());
        assert(false);
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void Game_destroyRenderer()
{
    if (!g_engine.sdl.renderer) return;
    assert(g_engine.sdl.textEngine);

    TTF_DestroyRendererTextEngine(g_engine.sdl.textEngine);
    g_engine.sdl.textEngine = NULL;
    SDL_DestroyRenderer(g_engine.sdl.renderer);
    g_engine.sdl.renderer = NULL;
}

void Game_destroyWindow()
{
    if (!g_engine.sdl.window) return;
    SDL_DestroyWindow(g_engine.sdl.window);
    g_engine.sdl.window = NULL;
}

void Game_destroyMixer()
{
    if (!g_engine.sdl.mixer) return;
    MIX_DestroyMixer(g_engine.sdl.mixer);
    g_engine.sdl.mixer = NULL;
}

void Game_quit()
{
    Game_destroyRenderer();
    Game_destroyWindow();
    Game_destroyMixer();
    Game_destroyStoragePath();

    Timer_destroy(g_engine.time);
    g_engine.time = NULL;

    TTF_Quit();
    MIX_Quit();
    SDL_Quit();
}

void Game_setRenderDrawColor(SDL_Color color, Uint8 alpha)
{
    assert(g_engine.sdl.renderer && "The renderer must be created");
    SDL_SetRenderDrawColor(g_engine.sdl.renderer, color.r, color.g, color.b, alpha);
}

void Memcpy(void* const dst, size_t dstSize, const void* src, size_t srcSize)
{
#ifdef _MSC_VER
    memcpy_s(dst, dstSize, src, srcSize);
#else
    memcpy(dst, src, (srcSize < dstSize) ? srcSize : dstSize);
#endif
}

char* Strdup(const char* src)
{
    size_t size = strlen(src);
    char* dst = (char*)calloc(size + 1, sizeof(char));
    ASSERT_NEW(dst);
    Memcpy(dst, size, src, size);
    return dst;
}

typedef struct ObfuscateAssetsContext
{
    SDL_Storage* srcStorage;
    SDL_Storage* dstStorage;
} ObfuscateAssetsContext;

static SDL_EnumerationResult Game_createAssetsFromDevCB(void* userdata, const char* dirname, const char* fname)
{
    ObfuscateAssetsContext* context = (ObfuscateAssetsContext*)userdata;
    bool success = false;
    SDL_PathInfo info = { 0 };
    char srcPath[256] = { 0 };
    char dstPath[256] = { 0 };
    SDL_strlcat(srcPath, dirname, sizeof(srcPath));
    SDL_strlcat(srcPath, fname, sizeof(srcPath));
    SDL_strlcat(dstPath, dirname, sizeof(dstPath));
    SDL_strlcat(dstPath, fname, sizeof(dstPath));
    int extIndex = -1;
    for (int i = (int)strlen(dstPath); i >= 0; i--)
    {
        if (dstPath[i] == '/' || dstPath[i] == '\\') break;
        if (dstPath[i] == '.')
        {
            extIndex = i;
            break;
        }
    }
    if (extIndex != -1) dstPath[extIndex] = '\0';
    SDL_strlcat(dstPath, ".dat", sizeof(dstPath));

    success = SDL_GetStoragePathInfo(context->srcStorage, srcPath, &info);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to get path info %s", SDL_GetError());
        return SDL_ENUM_FAILURE;
    }

    if (info.type == SDL_PATHTYPE_DIRECTORY)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Found directory: %s\n", srcPath);
        success = SDL_CreateStorageDirectory(context->dstStorage, srcPath);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to create directory %s", SDL_GetError());
            return SDL_ENUM_FAILURE;
        }
        SDL_EnumerateStorageDirectory(context->srcStorage, srcPath, Game_createAssetsFromDevCB, (void*)context);
    }
    else if (info.type == SDL_PATHTYPE_FILE)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Found file: %s\n", srcPath);
        Uint64 length = 0;
        success = SDL_GetStorageFileSize(context->srcStorage, srcPath, &length);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to get file size %s", SDL_GetError());
            return SDL_ENUM_FAILURE;
        }

        void* buffer = SDL_malloc(length);
        success = SDL_ReadStorageFile(context->srcStorage, srcPath, buffer, length);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to read file %s", SDL_GetError());
            SDL_free(buffer);
            return SDL_ENUM_FAILURE;
        }

        Game_obfuscateMem(buffer, length);
        success = SDL_WriteStorageFile(context->dstStorage, dstPath, buffer, length);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to write file %s", SDL_GetError());
            SDL_free(buffer);
            return SDL_ENUM_FAILURE;
        }
        SDL_free(buffer);
        buffer = NULL;
    }

    return SDL_ENUM_CONTINUE;
}

void Game_createAssetsFromDev()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Create assets from dev folder...\n");
    ObfuscateAssetsContext context = { 0 };
    char srcAssetsPath[256] = { 0 };
    char dstAssetsPath[256] = { 0 };
    SDL_strlcat(srcAssetsPath, g_engine.paths.projectRoot, sizeof(srcAssetsPath));
    SDL_strlcat(srcAssetsPath, "\\assets_dev", sizeof(srcAssetsPath));
    SDL_strlcat(dstAssetsPath, g_engine.paths.assets, sizeof(dstAssetsPath));

    context.srcStorage = SDL_OpenFileStorage(srcAssetsPath);
    context.dstStorage = SDL_OpenFileStorage(dstAssetsPath);
    if (context.srcStorage == NULL || context.dstStorage == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open storage %s\n", SDL_GetError());
        assert(false);
        return;
    }
    SDL_EnumerateStorageDirectory(context.srcStorage, NULL, Game_createAssetsFromDevCB, (void*)&context);
    SDL_CloseStorage(context.srcStorage);
    SDL_CloseStorage(context.dstStorage);
}

void Game_updateSizes()
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_engine.sdl.renderer, &viewport);

    g_engine.sizes.uiPixelsPerUnit = Vec2_set(
        viewport.w / g_engine.sizes.uiSize.x,
        viewport.h / g_engine.sizes.uiSize.y
    );
    g_engine.sizes.viewportSize = Vec2_set(
        (float)viewport.w,
        (float)viewport.h
    );
    g_engine.sizes.mainRenderScale = (float)viewport.w / g_engine.sizes.defaultRenderWidth;
}


void Game_obfuscateMem(void* memory, Uint64 size)
{
    char* buffer = (char*)memory;
    buffer[0] ^= 0x73;
    buffer[0] = 0xBB * buffer[0] + 0xC9;
    for (Uint64 i = 1; i < size; i++)
    {
        buffer[i] ^= buffer[i - 1];
        buffer[i] = 0xBB * buffer[i] + 0xC9;
    }
}

void Game_retriveMem(void* memory, Uint64 size)
{
    char* buffer = (char*)memory;
    for (Uint64 i = size - 1; i > 0; i--)
    {
        buffer[i] = 0x73 * (buffer[i] + 0x37);
        buffer[i] ^= buffer[i - 1];
    }
    buffer[0] = 0x73 * (buffer[0] + 0x37);
    buffer[0] ^= 0x73;
}
