//Bubble Shooter - main.cpp (reescrito y saneado)
// C++17 / SDL2


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "functions.h"
#include "bubble.h"
#include "entity.h"
#include "singletons.h"
#include "game.h"
#include "levelmanager.h"
#include "matchfinding.h"
#include "soundmanager.h"

namespace fs = std::filesystem;

constexpr int DESIGN_WIDTH  = 1280;
constexpr int DESIGN_HEIGHT = 800;
constexpr double PI = 3.14159265358979323846;
constexpr double TWO_PI = PI * 2.0;
constexpr double MAX_DT = 1.0 / 30.0;
constexpr double MIN_SHOT_LENGTH = 8.0;

bool g_initializedSDL = false;
bool g_initializedIMG = false;
bool g_initializedTTF = false;
bool g_initializedMixer = false;

std::string g_assetRoot = ".";

std::mt19937 g_rng{std::random_device{}()};

SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;

SDL_Texture* wall_img_day = nullptr;
SDL_Texture* wall_img_night = nullptr;
SDL_Texture* logo_asli_png = nullptr;
SDL_Texture* theme_dokme_png = nullptr;
SDL_Texture* regidter_dokme_png = nullptr;
SDL_Texture* leader_board_png = nullptr;
SDL_Texture* setting_window_png = nullptr;
SDL_Texture* exit_dokme_png = nullptr;
SDL_Texture* thunder_png = nullptr;
SDL_Texture* blur_day_version_jpg = nullptr;
SDL_Texture* blur_night_version_jpg = nullptr;
SDL_Texture* mute_dokme_png = nullptr;
SDL_Texture* unmute_dokme_png = nullptr;
SDL_Texture* quit_dokme_png = nullptr;
SDL_Texture* time_mode_dokme_png = nullptr;
SDL_Texture* cheat_png = nullptr;
SDL_Texture* normal_mode_dokme_png = nullptr;
SDL_Texture* setting_dokme_png = nullptr;
SDL_Texture* music_dokme_png = nullptr;
SDL_Texture* pause_dokme_png = nullptr;
SDL_Texture* start_dokme_png = nullptr;
SDL_Texture* canon = nullptr;
SDL_Texture* full_music_dokme_png = nullptr;
SDL_Texture* half_music_dokme_png = nullptr;
SDL_Texture* zero_music_dokme_png = nullptr;

Mix_Music* bg_sound = nullptr;

bool g_running = true;
bool g_setting = false;
bool g_menuRun = false;
bool g_registerMenu = false;
bool g_darkMode = false;
bool g_musicChanged = false;
int g_volume = 2;

bool g_normalMode = true;
bool g_startGame = false;

TTF_Font* g_menuFont = nullptr;

struct Rects
{
    SDL_Rect setting{};
    SDL_Rect thunder{};
    SDL_Rect music{};
    SDL_Rect quit{};
    SDL_Rect mute{};
    SDL_Rect exitGame{};
    SDL_Rect fullMusic{};
    SDL_Rect halfMusic{};
    SDL_Rect canon{};
    SDL_Rect zeroMusic{};
    SDL_Rect normalMode{};
    SDL_Rect timeMode{};
    SDL_Rect unmute{};
    SDL_Rect theme{};
    SDL_Rect leaderboard{};
    SDL_Rect registerButton{};
    SDL_Rect settingWindow{};
    SDL_Rect gameBg{};
    SDL_Rect blurDay{};
    SDL_Rect blurNight{};
    SDL_Rect pause{};
    SDL_Rect cheat{};
    SDL_Rect play{};
    SDL_Rect start{};
    SDL_Rect logo{};
    SDL_Rect wallDay{};
    SDL_Rect wallNight{};
} g_rect;

struct User
{
    std::string name;
    int score = 0;
};

fs::path resourcePath(const std::string& file)
{
    if (file.empty())
        return {};

    fs::path input(file);

    // Rutas absolutas se conservan tal cual.
    if (input.is_absolute())
        return input;

    const std::string name = input.filename().string();

    // --------------------------------------------------------
    // DATA
    // --------------------------------------------------------
    if (name == "config.txt" ||
        name == "levels.txt" ||
        name == "user.txt")
    {
        return fs::path(g_assetRoot) / "data" / name;
    }

    // --------------------------------------------------------
    // FONTS
    // --------------------------------------------------------
    if (input.extension() == ".ttf" ||
        input.extension() == ".TTF")
    {
        return fs::path(g_assetRoot) / "assets" / "fonts" / name;
    }

    // --------------------------------------------------------
    // AUDIO
    // --------------------------------------------------------
    if (input.extension() == ".mp3" ||
        input.extension() == ".MP3")
    {
        return fs::path(g_assetRoot) / "assets" / "audio" / "music" / name;
    }

    if (input.extension() == ".wav" ||
        input.extension() == ".WAV")
    {
        return fs::path(g_assetRoot) / "assets" / "audio" / "sounds" / name;
    }

    // --------------------------------------------------------
    // BUBBLES
    // --------------------------------------------------------
    if (name.rfind("bubble_", 0) == 0)
    {
        return fs::path(g_assetRoot) /
               "assets" / "images" / "bubbles" / name;
    }

    // --------------------------------------------------------
    // BACKGROUNDS
    // --------------------------------------------------------
    if (name == "day_version.jpg" ||
        name == "day_version_blego.jpg" ||
        name == "night_version.jpg" ||
        name == "night_version_blego.jpg" ||
        name == "blur_day_version.jpg" ||
        name == "blur_night_version.jpg")
    {
        return fs::path(g_assetRoot) /
               "assets" / "images" / "backgrounds" / name;
    }

    // --------------------------------------------------------
    // GAME
    // --------------------------------------------------------
    if (name.rfind("ball", 0) == 0 ||
        name == "canon.png" ||
        name == "cheat.png" ||
        name == "thunder.png" ||
        name == "cod_img.jpg" ||
        name == "cod_img - Copy.jpg" ||
        name == "test_2.jfif")
    {
        return fs::path(g_assetRoot) /
               "assets" / "images" / "game" / name;
    }

    // --------------------------------------------------------
    // UI
    // --------------------------------------------------------
    return fs::path(g_assetRoot) /
           "assets" / "images" / "ui" / name;
}

std::string pathOf(const std::string& file)
{
    return resourcePath(file).string();
}

bool fileExists(const std::string& file)
{
    std::error_code ec;
    return fs::is_regular_file(resourcePath(file), ec);
}

std::string findExisting(const std::initializer_list<const char*>& names)
{
    for (const char* name : names)
    {
        if (name && fileExists(name))
            return pathOf(name);
    }
    return {};
}

void logSDLError(const char* where)
{
    std::cerr << "[SDL] " << where << ": " << SDL_GetError() << '\n';
}

void logIMGError(const char* where)
{
    std::cerr << "[SDL_image] " << where << ": " << IMG_GetError() << '\n';
}

void logTTFError(const char* where)
{
    std::cerr << "[SDL_ttf] " << where << ": " << TTF_GetError() << '\n';
}

SDL_Texture* loadTexture(const char* file, bool required = false)
{
    if (!g_renderer || !file)
        return nullptr;

    const std::string full = pathOf(file);
    if (!fileExists(file))
    {
        std::cerr << "[ASSET] Falta: " << full << '\n';
        if (required)
            return nullptr;
        return nullptr;
    }

    SDL_Texture* texture = IMG_LoadTexture(g_renderer, full.c_str());
    if (!texture)
        logIMGError(full.c_str());
    return texture;
}

void destroyTexture(SDL_Texture*& texture)
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void destroyGlobalTextures()
{
    destroyTexture(wall_img_day);
    destroyTexture(wall_img_night);
    destroyTexture(logo_asli_png);
    destroyTexture(theme_dokme_png);
    destroyTexture(regidter_dokme_png);
    destroyTexture(leader_board_png);
    destroyTexture(setting_window_png);
    destroyTexture(exit_dokme_png);
    destroyTexture(thunder_png);
    destroyTexture(blur_day_version_jpg);
    destroyTexture(blur_night_version_jpg);
    destroyTexture(mute_dokme_png);
    destroyTexture(unmute_dokme_png);
    destroyTexture(quit_dokme_png);
    destroyTexture(time_mode_dokme_png);
    destroyTexture(cheat_png);
    destroyTexture(normal_mode_dokme_png);
    destroyTexture(setting_dokme_png);
    destroyTexture(music_dokme_png);
    destroyTexture(pause_dokme_png);
    destroyTexture(start_dokme_png);
    destroyTexture(canon);
    destroyTexture(full_music_dokme_png);
    destroyTexture(half_music_dokme_png);
    destroyTexture(zero_music_dokme_png);
}

bool initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        logSDLError("SDL_Init");
        return false;
    }
    g_initializedSDL = true;

    const int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
    {
        logIMGError("IMG_Init");
        return false;
    }
    g_initializedIMG = true;

    if (TTF_Init() != 0)
    {
        logTTFError("TTF_Init");
        return false;
    }
    g_initializedTTF = true;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    {
        std::cerr << "[SDL_mixer] Mix_OpenAudio: " << Mix_GetError() << '\n';
        return false;
    }
    g_initializedMixer = true;
    Mix_AllocateChannels(16);
    return true;
}

bool createMainWindow()
{
    g_window = SDL_CreateWindow(
        "Bubble Shooter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DESIGN_WIDTH,
        DESIGN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!g_window)
    {
        logSDLError("SDL_CreateWindow");
        return false;
    }

    g_renderer = SDL_CreateRenderer(
        g_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!g_renderer)
    {
        // Fallback to software rendering.
        g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (!g_renderer)
    {
        logSDLError("SDL_CreateRenderer");
        return false;
    }

    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_RenderSetLogicalSize(g_renderer, DESIGN_WIDTH, DESIGN_HEIGHT);

    GUIManager::getInstance()->SetWindow(g_window);
    GUIManager::getInstance()->SetRenderer(g_renderer);
    return true;
}

void loadGlobalTextures()
{
    wall_img_day = loadTexture("day_version_blego.jpg");
    wall_img_night = loadTexture("night_version_blego.jpg");
    logo_asli_png = loadTexture("logo_asli.png");
    start_dokme_png = loadTexture("start_dokme.png");
    mute_dokme_png = loadTexture("mute_dokme.png");
    unmute_dokme_png = loadTexture("unmute_dokme.png");
    quit_dokme_png = loadTexture("quit_dokme.png");
    pause_dokme_png = loadTexture("pause_dokme.png");
    setting_dokme_png = loadTexture("setting_dokme.png");
    thunder_png = loadTexture("thunder.png");
    music_dokme_png = loadTexture("music_dokme.png");
    canon = loadTexture("canon.png");
    cheat_png = loadTexture("cheat.png");
    theme_dokme_png = loadTexture("theme_dokme.png");
    leader_board_png = loadTexture("leader_board_dokme.png");
    if (!leader_board_png)
        leader_board_png = loadTexture("leader_board.png");
    regidter_dokme_png = loadTexture("register_dokme.png");
    setting_window_png = loadTexture("test_1.png");
    normal_mode_dokme_png = loadTexture("normal_mode_dokme.png");
    time_mode_dokme_png = loadTexture("time_mode_dokme.png");
    exit_dokme_png = loadTexture("exit_dokme.png");
    full_music_dokme_png = loadTexture("full_music_dokme.png");
    half_music_dokme_png = loadTexture("half_music_dokme.png");
    zero_music_dokme_png = loadTexture("zero_music_dokme.png");
    blur_day_version_jpg = loadTexture("blur_day_version.jpg");
    blur_night_version_jpg = loadTexture("blur_night_version.jpg");
}

void setupRects()
{
    g_rect.setting = {577, 30, 150, 240};
    g_rect.thunder = {0, 0, 0, 0};
    g_rect.music = {530, 460, 250, 80};
    g_rect.quit = {70, 100, 150, 240};
    g_rect.mute = {95, 630, 70, 150};
    g_rect.exitGame = {70, 100, 150, 240};
    g_rect.fullMusic = {505, 210, 295, 240};
    g_rect.halfMusic = {505, 210, 295, 240};
    g_rect.canon = {0, 0, 130, 130};
    g_rect.zeroMusic = {505, 210, 295, 240};
    g_rect.normalMode = {570, 300, 180, 80};
    g_rect.timeMode = {380, 400, 180, 80};
    g_rect.unmute = {95, 560, 70, 150};
    g_rect.theme = {70, 340, 150, 240};
    g_rect.leaderboard = {0, 0, 0, 0};
    g_rect.registerButton = {760, 320, 180, 240};
    g_rect.settingWindow = {0, -100, 1300, 1000};
    g_rect.gameBg = {0, 0, DESIGN_WIDTH, DESIGN_HEIGHT};
    g_rect.blurDay = {0, 0, DESIGN_WIDTH, DESIGN_HEIGHT};
    g_rect.blurNight = {0, 0, DESIGN_WIDTH, DESIGN_HEIGHT};
    g_rect.pause = {930, 60, 190, 70};
    g_rect.cheat = {900, 250, 320, 350};
    g_rect.play = {0, 0, 0, 0};
    g_rect.start = {520, 390, 240, 300};
    g_rect.logo = {430, 80, 450, 380};
    g_rect.wallDay = {0, 0, DESIGN_WIDTH, DESIGN_HEIGHT};
    g_rect.wallNight = {0, 0, DESIGN_WIDTH, DESIGN_HEIGHT};
}

void applyMusicVolume()
{
    if (!g_initializedMixer)
        return;

    int volume = MIX_MAX_VOLUME;
    if (g_volume == 0)
        volume = 0;
    else if (g_volume == 1)
        volume = MIX_MAX_VOLUME / 3;

    Mix_VolumeMusic(volume);
}

void playMusic(const char* filename)
{
    if (!g_initializedMixer || !filename)
        return;

    const std::string full = pathOf(filename);
    if (!fileExists(filename))
    {
        std::cerr << "[MUSIC] Falta: " << full << '\n';
        return;
    }

    Mix_Music* newMusic = Mix_LoadMUS(full.c_str());
    if (!newMusic)
    {
        std::cerr << "[SDL_mixer] Mix_LoadMUS " << full << ": " << Mix_GetError() << '\n';
        return;
    }

    Mix_HaltMusic();
    if (bg_sound)
        Mix_FreeMusic(bg_sound);
    bg_sound = newMusic;
    applyMusicVolume();
    Mix_PlayMusic(bg_sound, -1);
}

void cleanupAudio()
{
    if (bg_sound)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bg_sound);
        bg_sound = nullptr;
    }

    if (g_initializedMixer)
    {
        Mix_CloseAudio();
        g_initializedMixer = false;
    }
}

void cleanupSDL()
{
    destroyGlobalTextures();

    if (g_menuFont)
    {
        TTF_CloseFont(g_menuFont);
        g_menuFont = nullptr;
    }

    if (g_renderer)
    {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = nullptr;
    }

    if (g_window)
    {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }

    if (g_initializedIMG)
    {
        IMG_Quit();
        g_initializedIMG = false;
    }

    if (g_initializedTTF)
    {
        TTF_Quit();
        g_initializedTTF = false;
    }

    if (g_initializedSDL)
    {
        SDL_Quit();
        g_initializedSDL = false;
    }
}

std::vector<User> readUsers(const std::string& filename)
{
    std::vector<User> users;
    const std::string full = pathOf(filename);
    std::ifstream file(full);
    if (!file)
        return users;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::size_t pos = line.find_last_of(" \t");
        if (pos == std::string::npos)
            continue;

        User user;
        user.name = line.substr(0, pos);
        try
        {
            user.score = std::stoi(line.substr(pos + 1));
            users.push_back(user);
        }
        catch (...)
        {
            // Ignora líneas corruptas del archivo de puntuaciones.
        }
    }

    std::sort(users.begin(), users.end(), [](const User& a, const User& b)
    {
        return a.score > b.score;
    });

    return users;
}

void showLeaderboard(SDL_Renderer* renderer,
                     const std::string& filename,
                     TTF_Font* font,
                     SDL_Color color)
{
    if (!renderer || !font)
        return;

    const auto users = readUsers(filename);
    int y = 55;
    constexpr int x = 930;
    constexpr int maxRows = 10;

    int row = 0;
    for (const User& user : users)
    {
        if (row++ >= maxRows)
            break;

        const std::string text = user.name + "  " + std::to_string(user.score);
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        if (!surface)
            continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture)
        {
            SDL_Rect dst{x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
        y += 31;
    }
}

bool pointInRect(int px, int py, const SDL_Rect& r)
{
    return px >= r.x && py >= r.y && px < r.x + r.w && py < r.y + r.h;
}

bool is_clicked(int x1, int y1, int x, int y, int w, int h)
{
    // El proyecto original usaba solamente el tercio inferior de cada botón.
    // Conservamos esa zona de interacción para no cambiar la interfaz visual.
    h /= 3;
    y += h;
    return x1 > x && x1 < x + w && y1 > y && y1 < y + h;
}

void draw(SDL_Renderer* renderer,
          SDL_Texture* texture,
          SDL_Rect /*unusedRect*/,
          int x,
          int y,
          int w,
          int h)
{
    if (!renderer || !texture || w <= 0 || h <= 0)
        return;

    SDL_Rect dst{x, y, w, h};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
}

void renderText(SDL_Renderer* renderer,
                const char* text,
                int x,
                int y,
                int w,
                int h)
{
    if (!renderer || !text || !g_menuFont)
        return;

    SDL_Color color{255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(g_menuFont, text, color);
    if (!surface)
        return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture)
    {
        SDL_Rect dst{x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void renderMenuBackground(SDL_Renderer* renderer, bool day)
{
    if (day)
        draw(renderer, wall_img_day, g_rect.wallDay, 0, 0, DESIGN_WIDTH, DESIGN_HEIGHT);
    else
        draw(renderer, wall_img_night, g_rect.wallNight, 0, 0, DESIGN_WIDTH, DESIGN_HEIGHT);
}

void menu(SDL_Renderer* renderer, bool day)
{
    if (!renderer)
        return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderMenuBackground(renderer, day);

    draw(renderer, logo_asli_png, g_rect.logo, 430, 80, 450, 380);
    draw(renderer, start_dokme_png, g_rect.start, 520, 390, 240, 300);
    draw(renderer, quit_dokme_png, g_rect.quit, 70, 100, 150, 240);
    draw(renderer, setting_dokme_png, g_rect.setting, 70, 220, 150, 240);
    draw(renderer, theme_dokme_png, g_rect.theme, 70, 340, 150, 240);

    showLeaderboard(renderer, "user.txt", g_menuFont, SDL_Color{255, 255, 255, 255});
}

void drawSettings(SDL_Renderer* renderer, bool day)
{
    if (!renderer)
        return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (day)
        draw(renderer, blur_day_version_jpg, g_rect.blurDay, 0, 0, DESIGN_WIDTH, DESIGN_HEIGHT);
    else
        draw(renderer, blur_night_version_jpg, g_rect.blurNight, 0, 0, DESIGN_WIDTH, DESIGN_HEIGHT);

    draw(renderer, setting_window_png, g_rect.settingWindow, 0, -100, 1300, 1000);
    draw(renderer, unmute_dokme_png, g_rect.unmute, 95, 560, 70, 150);
    draw(renderer, mute_dokme_png, g_rect.mute, 95, 630, 70, 150);

    if (g_volume == 0)
        draw(renderer, zero_music_dokme_png, g_rect.zeroMusic, 505, 210, 295, 240);
    else if (g_volume == 1)
        draw(renderer, half_music_dokme_png, g_rect.halfMusic, 505, 210, 295, 240);
    else
        draw(renderer, full_music_dokme_png, g_rect.fullMusic, 505, 210, 295, 240);

    draw(renderer, setting_dokme_png, g_rect.setting, 577, 30, 150, 240);
    draw(renderer, music_dokme_png, g_rect.music, 530, 460, 250, 80);
    draw(renderer, exit_dokme_png, g_rect.exitGame, 70, 100, 150, 240);
}

void setting_ma(SDL_Renderer* renderer, bool mode)
{
    drawSettings(renderer, mode);
}

void setting_ma_actions(SDL_Renderer* renderer)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            g_setting = false;
            g_running = false;
            return;
        }

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
            continue;

        const int x = event.button.x;
        const int y = event.button.y;

        if (is_clicked(x, y, 70, 100, 150, 240))
        {
            g_setting = false;
            return;
        }

        if (is_clicked(x, y, 505, 210, 100, 240))
            g_volume = 0;
        else if (is_clicked(x, y, 605, 210, 100, 240))
            g_volume = 1;
        else if (is_clicked(x, y, 705, 210, 95, 240))
            g_volume = 2;

        if (is_clicked(x, y, 95, 630, 70, 150))
            g_volume = 0;
        else if (is_clicked(x, y, 95, 560, 70, 150))
            g_volume = 2;

        if (is_clicked(x, y, 530, 460, 250, 80))
        {
            g_musicChanged = !g_musicChanged;
            playMusic(g_musicChanged ? "paszamine2.mp3" : "paszamine.mp3");
        }
    }

    applyMusicVolume();
}

void menu_run(SDL_Renderer* renderer, bool mode)
{
    if (!renderer)
        return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (mode)
        draw(renderer, blur_day_version_jpg, g_rect.blurDay, 0, 0, DESIGN_WIDTH, DESIGN_HEIGHT);
    else
        draw(renderer, blur_night_version_jpg, g_rect.blurNight, 0, 0, DESIGN_WIDTH, DESIGN_HEIGHT);

    draw(renderer, setting_window_png, g_rect.settingWindow, 0, -100, 1300, 1000);
    draw(renderer, normal_mode_dokme_png, g_rect.normalMode, 570, 300, 180, 80);
    draw(renderer, time_mode_dokme_png, g_rect.timeMode, 380, 400, 180, 80);
    draw(renderer, exit_dokme_png, g_rect.exitGame, 70, 100, 150, 240);
    draw(renderer, regidter_dokme_png, g_rect.registerButton, 760, 320, 180, 240);
}

void menu_run_actions(SDL_Renderer* renderer)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            g_menuRun = false;
            g_running = false;
            return;
        }

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
            continue;

        const int x = event.button.x;
        const int y = event.button.y;

        if (is_clicked(x, y, 380, 400, 180, 80))
        {
            g_normalMode = false;
            g_startGame = true;
            g_menuRun = false;
            return;
        }

        if (is_clicked(x, y, 570, 300, 180, 80))
        {
            g_normalMode = true;
            g_startGame = true;
            g_menuRun = false;
            return;
        }

        if (is_clicked(x, y, 70, 100, 150, 240))
        {
            g_menuRun = false;
            return;
        }

        if (is_clicked(x, y, 760, 320, 180, 240))
        {
            g_registerMenu = true;
            g_menuRun = false;
            return;
        }
    }
}

void saveUser(const std::string& name)
{
    if (name.empty())
        return;

    std::ofstream file(pathOf("user.txt"), std::ios::app);
    if (file)
        file << name << " 0\n";
}

void user_input()
{
    SDL_StartTextInput();

    bool active = true;
    std::string input;
    Uint32 lastFrame = SDL_GetTicks();

    while (active && g_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                active = false;
                g_running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    if (!input.empty())
                    {
                        saveUser(input);
                        active = false;
                    }
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    active = false;
                }
                else if (event.key.keysym.sym == SDLK_BACKSPACE && !input.empty())
                {
                    input.pop_back();
                }
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                // Limita la longitud para evitar una cadena absurda.
                if (input.size() < 20)
                    input += event.text.text;
            }
        }

        SDL_SetRenderDrawColor(g_renderer, 20, 20, 25, 255);
        SDL_RenderClear(g_renderer);
        renderText(g_renderer, "Please enter your name:", 100, 100, 500, 70);
        renderText(g_renderer, input.c_str(), 100, 200, 500, 70);
        renderText(g_renderer, "ENTER = save     ESC = cancel", 100, 300, 650, 60);
        SDL_RenderPresent(g_renderer);

        const Uint32 now = SDL_GetTicks();
        if (now - lastFrame < 16)
            SDL_Delay(16 - (now - lastFrame));
        lastFrame = now;
    }

    SDL_StopTextInput();
    g_registerMenu = false;
}

void barikalla()
{
    bool active = true;
    while (active && g_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                active = false;
                g_running = false;
            }
            else if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN)
            {
                active = false;
            }
        }

        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
        SDL_RenderClear(g_renderer);
        renderText(g_renderer, "Barikalla!", 350, 250, 300, 100);
        renderText(g_renderer, "Press a key or click", 350, 380, 400, 60);
        SDL_RenderPresent(g_renderer);
        SDL_Delay(8);
    }
}

// -----------------------------------------------------------------------------
// SoundManager
// -----------------------------------------------------------------------------

SoundManager* SoundManager::instance = nullptr;

SoundManager::SoundManager()
{
    std::fill(std::begin(deviceId), std::end(deviceId), 0);
    std::fill(std::begin(wavLength), std::end(wavLength), 0);
    std::fill(std::begin(wavBuffer), std::end(wavBuffer), nullptr);
    std::memset(wavSpec, 0, sizeof(wavSpec));

    const std::array<const char*, NUM_SOUNDS> files = {
        "assets_sounds_shoot_bubble.wav",
        "assets_sounds_failed_shot.wav",
        "assets_sounds_combo.wav",
        "assets_sounds_combo_bonus1.wav",
        "assets_sounds_combo_bonus2.wav",
        "assets_sounds_fully_loaded.wav",
        "assets_sounds_hit_wall.wav",
        "assets_sounds_level_up.wav",
        "assets_sounds_reset_bonus.wav"
    };

    for (int i = 0; i < NUM_SOUNDS; ++i)
        Load(files[i], static_cast<SoundType>(i));
}

void SoundManager::Load(const char* filename, SoundType soundType)
{
    if (!filename)
        return;

    const std::string full = pathOf(filename);
    if (!fileExists(filename))
    {
        std::cerr << "[SOUND] Falta: " << full << '\n';
        return;
    }

    if (SDL_LoadWAV(full.c_str(), &wavSpec[soundType], &wavBuffer[soundType], &wavLength[soundType]) == nullptr)
    {
        std::cerr << "[SOUND] " << full << ": " << SDL_GetError() << '\n';
        return;
    }

    deviceId[soundType] = SDL_OpenAudioDevice(nullptr, 0, &wavSpec[soundType], nullptr, 0);
    if (deviceId[soundType] == 0)
    {
        std::cerr << "[SOUND] SDL_OpenAudioDevice: " << SDL_GetError() << '\n';
        SDL_FreeWAV(wavBuffer[soundType]);
        wavBuffer[soundType] = nullptr;
        wavLength[soundType] = 0;
    }
}

void SoundManager::PlaySound(SoundType soundType)
{
    if (soundType < 0 || soundType >= NUM_SOUNDS)
        return;

    const int i = static_cast<int>(soundType);
    if (!wavBuffer[i] || wavLength[i] == 0 || deviceId[i] == 0)
        return;

    // Limpia audio pendiente para no acumular cientos de reproducciones si el
    // jugador dispara rápidamente.
    SDL_ClearQueuedAudio(deviceId[i]);
    SDL_QueueAudio(deviceId[i], wavBuffer[i], wavLength[i]);
    SDL_PauseAudioDevice(deviceId[i], 0);
}

SoundManager::~SoundManager()
{
    for (int i = 0; i < NUM_SOUNDS; ++i)
    {
        if (deviceId[i] != 0)
        {
            SDL_ClearQueuedAudio(deviceId[i]);
            SDL_CloseAudioDevice(deviceId[i]);
            deviceId[i] = 0;
        }

        if (wavBuffer[i])
        {
            SDL_FreeWAV(wavBuffer[i]);
            wavBuffer[i] = nullptr;
        }
    }
    instance = nullptr;
}

SoundManager* SoundManager::getInstance()
{
    if (!instance)
        instance = new SoundManager();
    return instance;
}

// -----------------------------------------------------------------------------
// GUI / Font / Texture managers
// -----------------------------------------------------------------------------

GUIManager* GUIManager::instance = nullptr;

GUIManager::GUIManager() : window(nullptr), renderer(nullptr) {}

GUIManager::~GUIManager()
{
    window = nullptr;
    renderer = nullptr;
    instance = nullptr;
}

GUIManager* GUIManager::getInstance()
{
    if (!instance)
        instance = new GUIManager();
    return instance;
}

FontRenderer* FontRenderer::instance = nullptr;

FontRenderer::FontRenderer() : font(nullptr)
{
    if (!g_initializedTTF)
    {
        if (TTF_Init() == 0)
            g_initializedTTF = true;
    }

    std::string fontPath = findExisting({"Sans.ttf", "ARIAL.TTF", "arial.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"});
    if (fontPath.empty())
    {
        std::cerr << "[FONT] No se encontró una fuente.\n";
        return;
    }

    font = TTF_OpenFont(fontPath.c_str(), 36);
    if (!font)
        logTTFError(fontPath.c_str());
}

FontRenderer::~FontRenderer()
{
    if (font)
    {
        TTF_CloseFont(font);
        font = nullptr;
    }
    instance = nullptr;
}

FontRenderer* FontRenderer::getInstance()
{
    if (!instance)
        instance = new FontRenderer();
    return instance;
}

void FontRenderer::DisplayText(const char* msg, int x, int y)
{
    if (!msg || !font)
        return;

    SDL_Renderer* renderer = GUIManager::getInstance()->GetRenderer();
    if (!renderer)
        return;

    SDL_Color color{0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, msg, color);
    if (!surface)
        return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture)
    {
        SDL_Rect rect{x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

TextureManager* TextureManager::instance = nullptr;

TextureManager::TextureManager() : background(nullptr), bubbles{}
{
    background = CreateTextureFromImage("test_2.jfif");

    const std::array<const char*, NUM_BUBBLES> colors = {
        "blue", "yellow", "green", "red", "transparent"
    };

    for (int i = 0; i < NUM_BUBBLES; ++i)
    {
        const std::string file = "bubble_" + std::string(colors[i]) + ".png";
        bubbles[i] = CreateTextureFromImage(file.c_str());
    }
}

TextureManager::~TextureManager()
{
    destroyTexture(background);
    for (SDL_Texture*& texture : bubbles)
        destroyTexture(texture);
    instance = nullptr;
}

TextureManager* TextureManager::getInstance()
{
    if (!instance)
        instance = new TextureManager();
    return instance;
}

SDL_Texture* TextureManager::CreateTextureFromImage(const char* path)
{
    if (!g_renderer || !path)
        return nullptr;

    const std::string full = pathOf(path);
    if (!fileExists(path))
    {
        std::cerr << "[TEXTURE] Falta: " << full << '\n';
        return nullptr;
    }

    SDL_Surface* surface = IMG_Load(full.c_str());
    if (!surface)
    {
        logIMGError(full.c_str());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);
    if (!texture)
        logSDLError("SDL_CreateTextureFromSurface");

    SDL_FreeSurface(surface);
    return texture;
}

// -----------------------------------------------------------------------------
// Matchfinder
// -----------------------------------------------------------------------------

Matchfinder* Matchfinder::instance = nullptr;

Matchfinder* Matchfinder::getInstance()
{
    if (!instance)
        instance = new Matchfinder();
    return instance;
}

Matchfinder::Matchfinder() : graph{}, matrix{}, numBubbles(0), path{}
{
    ZeroMatrix();
}

Matchfinder::~Matchfinder()
{
    for (Path* p : path)
        delete p;
    path.clear();
    instance = nullptr;
}

void Matchfinder::ZeroMatrix()
{
    for (auto& row : matrix)
        std::fill(std::begin(row), std::end(row), static_cast<int8_t>(BUBBLE_NONE));

    numBubbles = 0;

    for (Path* p : path)
        delete p;
    path.clear();
}

void Matchfinder::SetMatrix(int i, int j, BubbleColor bubbleColor)
{
    if (i < 0 || i >= BOARD_SIZE_X || j < 0 || j >= BOARD_SIZE_Y)
        return;

    const int8_t old = matrix[i][j];
    const int8_t value = static_cast<int8_t>(bubbleColor);

    const bool oldCounts = old >= BUBBLE_BLUE && old <= BUBBLE_RED;
    const bool newCounts = value >= BUBBLE_BLUE && value <= BUBBLE_RED;

    if (oldCounts && !newCounts)
        --numBubbles;
    else if (!oldCounts && newCounts)
        ++numBubbles;

    matrix[i][j] = value;
}

int8_t Matchfinder::GetMatrix(int i, int j) const
{
    if (i < 0 || i >= BOARD_SIZE_X || j < 0 || j >= BOARD_SIZE_Y)
        return static_cast<int8_t>(BUBBLE_NONE);
    return matrix[i][j];
}

bool Matchfinder::BoardContains(BubbleColor bubbleColor)
{
    const int8_t target = static_cast<int8_t>(bubbleColor);
    for (const auto& row : matrix)
        for (int8_t value : row)
            if (value == target)
                return true;
    return false;
}

namespace
{
std::vector<std::pair<int,int>> neighborsFor(int x, int y)
{
    std::vector<std::pair<int,int>> result;
    result.reserve(6);

    const auto add = [&](int nx, int ny)
    {
        if (nx >= 0 && nx < BOARD_SIZE_X && ny >= 0 && ny < BOARD_SIZE_Y)
            result.emplace_back(nx, ny);
    };

    add(x, y - 1);
    add(x, y + 1);
    add(x - 1, y);
    add(x + 1, y);

    if ((x & 1) == 0)
    {
        add(x - 1, y - 1);
        add(x + 1, y - 1);
    }
    else
    {
        add(x - 1, y + 1);
        add(x + 1, y + 1);
    }

    return result;
}
}

bool Matchfinder::PathContains(std::vector<Path*>& p, int x, int y)
{
    return std::any_of(p.begin(), p.end(), [x, y](const Path* node)
    {
        return node && node->x == x && node->y == y;
    });
}

bool Matchfinder::CheckForMatch(int new_i, int new_j)
{
    for (Path* p : path)
        delete p;
    path.clear();

    if (new_i < 0 || new_i >= BOARD_SIZE_X || new_j < 0 || new_j >= BOARD_SIZE_Y)
        return false;

    const int8_t target = matrix[new_i][new_j];
    if (target < BUBBLE_BLUE || target > BUBBLE_RED)
        return false;

    std::array<std::array<bool, BOARD_SIZE_Y>, BOARD_SIZE_X> visited{};
    std::vector<std::pair<int,int>> queue;
    queue.emplace_back(new_i, new_j);
    visited[new_i][new_j] = true;

    for (std::size_t head = 0; head < queue.size(); ++head)
    {
        const auto [x, y] = queue[head];
        for (const auto [nx, ny] : neighborsFor(x, y))
        {
            if (visited[nx][ny] || matrix[nx][ny] != target)
                continue;
            visited[nx][ny] = true;
            queue.emplace_back(nx, ny);
        }
    }

    if (queue.size() < 3)
        return false;

    for (const auto [x, y] : queue)
        path.push_back(new Path(x, y, true));

    return true;
}

bool Matchfinder::IsIsolated(int x, int y)
{
    if (x < 0 || x >= BOARD_SIZE_X || y < 0 || y >= BOARD_SIZE_Y)
        return false;

    if (matrix[x][y] == BUBBLE_NONE)
        return false;

    // Las burbujas transparentes representan puntos de anclaje.
    std::array<std::array<bool, BOARD_SIZE_Y>, BOARD_SIZE_X> visited{};
    std::vector<std::pair<int,int>> queue;
    queue.emplace_back(x, y);
    visited[x][y] = true;

    for (std::size_t head = 0; head < queue.size(); ++head)
    {
        const auto [cx, cy] = queue[head];
        if (matrix[cx][cy] == BUBBLE_TRANSPARENT)
            return false;

        for (const auto [nx, ny] : neighborsFor(cx, cy))
        {
            if (visited[nx][ny])
                continue;
            if (matrix[nx][ny] == BUBBLE_NONE)
                continue;
            visited[nx][ny] = true;
            queue.emplace_back(nx, ny);
        }
    }

    return true;
}

void Matchfinder::ClearPath(Bubble* board[BOARD_SIZE_X][BOARD_SIZE_Y], int* pscore, int* pbubbles)
{
    if (pscore)
        *pscore = 0;
    if (pbubbles)
        *pbubbles = numBubbles;

    if (!board)
        return;

    const int startBubbles = numBubbles;

    for (Path* p : path)
    {
        if (!p)
            continue;
        const int x = p->x;
        const int y = p->y;
        if (x >= 0 && x < BOARD_SIZE_X && y >= 0 && y < BOARD_SIZE_Y && board[x][y])
        {
            board[x][y]->Clear();
            matrix[x][y] = static_cast<int8_t>(BUBBLE_NONE);
        }
        delete p;
    }
    path.clear();

    // Recalcula el conteo para que no dependa de llamadas previas.
    numBubbles = 0;
    for (int i = 0; i < BOARD_SIZE_X; ++i)
        for (int j = 0; j < BOARD_SIZE_Y; ++j)
            if (matrix[i][j] >= BUBBLE_BLUE && matrix[i][j] <= BUBBLE_RED)
                ++numBubbles;

    // Elimina grupos que ya no están conectados a un anclaje transparente.
    for (int i = 0; i < BOARD_SIZE_X; ++i)
    {
        for (int j = 0; j < BOARD_SIZE_Y; ++j)
        {
            if (!board[i][j])
                continue;
            const BubbleColor color = board[i][j]->GetColor();
            if (color == BUBBLE_NONE || color == BUBBLE_TRANSPARENT)
                continue;

            if (IsIsolated(i, j))
            {
                board[i][j]->Clear();
                matrix[i][j] = static_cast<int8_t>(BUBBLE_NONE);
            }
        }
    }

    numBubbles = 0;
    for (int i = 0; i < BOARD_SIZE_X; ++i)
        for (int j = 0; j < BOARD_SIZE_Y; ++j)
            if (matrix[i][j] >= BUBBLE_BLUE && matrix[i][j] <= BUBBLE_RED)
                ++numBubbles;

    const int broken = std::max(0, startBubbles - numBubbles);

    // Puntuación escalonada, conservando la idea original.
    int score = 0;
    if (broken > 0)
        score = 100;
    for (int i = 3; i < broken && i < 6; ++i)
        score += 50;
    for (int i = 6; i < broken && i < 9; ++i)
        score += 100;
    for (int i = 9; i < broken && i < 13; ++i)
        score += 150;
    for (int i = 13; i < broken && i < 16; ++i)
        score += 200;
    for (int i = 16; i < broken && i < 17; ++i)
        score += 300;
    for (int i = 17; i < broken; ++i)
        score += 300;

    if (pscore)
        *pscore = score;
    if (pbubbles)
        *pbubbles = numBubbles;
}

void Matchfinder::PrintMatrix(void)
{
    for (int j = 0; j < BOARD_SIZE_Y; ++j)
    {
        for (int i = 0; i < BOARD_SIZE_X; ++i)
            std::cout << static_cast<int>(matrix[i][j]) << ' ';
        std::cout << '\n';
    }
}

// -----------------------------------------------------------------------------
// LevelManager
// -----------------------------------------------------------------------------

LevelManager* LevelManager::instance = nullptr;

LevelManager::LevelManager() : level{}, numBubbles{}
{
    const std::string file = pathOf("levels.txt");
    FILE* fp = std::fopen(file.c_str(), "r");
    if (!fp)
    {
        std::cerr << "[LEVELS] No se pudo abrir " << file << '\n';
        return;
    }

    for (int l = 0; l < NUM_LEVELS_IMPLEMENTED; ++l)
    {
        for (int i = 0; i < BOARD_SIZE_X; ++i)
        {
            for (int j = 0; j < BOARD_SIZE_Y; ++j)
            {
                int value = 0;
                if (std::fscanf(fp, "%d", &value) != 1)
                    value = 0;
                value = std::clamp(value, 0, 2);
                level[l][i][j] = static_cast<int8_t>(value);
                if (value == 1)
                    ++numBubbles[l];
            }
        }
    }

    std::fclose(fp);
}

LevelManager::~LevelManager()
{
    instance = nullptr;
}

LevelManager* LevelManager::getInstance()
{
    if (!instance)
        instance = new LevelManager();
    return instance;
}

void LevelManager::SetLevel(int l, Bubble* board[BOARD_SIZE_X][BOARD_SIZE_Y])
{
    if (!board || l < 1 || l > NUM_LEVELS_IMPLEMENTED)
        return;

    Matchfinder::getInstance()->ZeroMatrix();

    static const BubbleColor colors[4] = {
        BUBBLE_BLUE,
        BUBBLE_YELLOW,
        BUBBLE_GREEN,
        BUBBLE_RED
    };

    SDL_FRect rect{
        static_cast<float>(BUBBLE_OFFSET_X),
        static_cast<float>(BUBBLE_OFFSET_Y),
        static_cast<float>(BUBBLE_WIDTH),
        static_cast<float>(BUBBLE_HEIGHT)
    };

    for (int i = 0; i < BOARD_SIZE_X; ++i)
    {
        rect.x = static_cast<float>(BUBBLE_OFFSET_X + ((i & 1) ? BUBBLE_WIDTH / 2 : 0));
        rect.y = static_cast<float>(BUBBLE_OFFSET_Y + i * (BUBBLE_HEIGHT - 5));

        for (int j = 0; j < BOARD_SIZE_Y; ++j)
        {
            delete board[i][j];
            board[i][j] = nullptr;

            const int8_t type = level[l - 1][i][j];
            if (type == 1)
                board[i][j] = new Bubble(colors[std::uniform_int_distribution<int>(0, 3)(g_rng)], rect, true);
            else if (type == 2)
                board[i][j] = new Bubble(BUBBLE_TRANSPARENT, rect, true);
            else
                board[i][j] = new Bubble(BUBBLE_NONE, rect, false);

            Matchfinder::getInstance()->SetMatrix(i, j, board[i][j]->GetColor());
            rect.x += BUBBLE_WIDTH;
        }
    }
}

// -----------------------------------------------------------------------------
// Entity / Bubble
// -----------------------------------------------------------------------------

bool Entity::rectangleVisible = false;

Entity::Entity()
    : texture(nullptr), srcRect{0, 0, 0, 0}, dstRect{0, 0, 0, 0}, isVisible(false)
{
}

Entity::~Entity() = default;

void Entity::Render()
{
    SDL_Renderer* renderer = GUIManager::getInstance()->GetRenderer();
    if (!renderer)
        return;

    if (texture && isVisible)
        SDL_RenderCopyF(renderer, texture, nullptr, &dstRect);

    if (rectangleVisible)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRectF(renderer, &dstRect);
    }
}

Bubble::Bubble(BubbleColor bubbleColor, SDL_FRect rect, bool visible)
    : Entity(), bubbleColor(bubbleColor)
{
    dstRect = rect;
    isVisible = visible;
    texture = TextureManager::getInstance()->GetBubble(bubbleColor);
}

Bubble::~Bubble() = default;

void Bubble::Clear()
{
    isVisible = false;
    texture = nullptr;
    bubbleColor = BUBBLE_NONE;
}

// -----------------------------------------------------------------------------
// Game
// -----------------------------------------------------------------------------

namespace
{
BubbleColor randomPlayableColor()
{
    return static_cast<BubbleColor>(std::uniform_int_distribution<int>(0, NUM_BUBBLES - 2)(g_rng));
}
}

Game::Game(const char* /*title*/, int /*xpos*/, int /*ypos*/, int /*width*/, int /*height*/, bool /*fullscreen*/)
    : running(true),
      paused(false),
      frozen(false),
      currentTime(std::time(nullptr)),
      levelOffset(1),
      level(1),
      score(0),
      currentFrame(0),
      frozenFrame(0),
      bubbles{},
      cannonBubble(nullptr),
      spareBubble(nullptr),
      shotBubble(nullptr),
      consecutiveStrikesRect{},
      cannonPoint{},
      mousePoint{},
      startPoint{},
      endPoint{},
      shotBubbleDirectionVector{},
      numBubblesLeft(0),
      timer(60),
      bubbleSpeed(15.0f),
      consecutiveStrikes(0),
      strikesAfter5Consecutive(0),
      scoreMultiplier(1.0f),
      t(0.0f),
      rotateLeft(false),
      rotateRight(false),
      shouldShootBubble(false),
      shootingBubble(false),
      velocity(0.0),
      acceleration(0.005)
{
    InitStats();
    InitMap();
    SDL_GetMouseState(&mousePoint.x, &mousePoint.y);
}

void Game::InitStats()
{
    currentFrame = 0;
    frozenFrame = 0;
    strikesAfter5Consecutive = 0;
    consecutiveStrikes = 0;
    consecutiveStrikesRect = {STRIKES_RECT_X, STRIKES_RECT_Y, STRIKES_RECT_W, 0};

    levelOffset = 1;
    t = 0.0f;
    velocity = 0.0;
    acceleration = 0.005;

    running = true;
    paused = false;
    frozen = false;
    shouldShootBubble = false;
    shootingBubble = false;

    level = 1;
    score = 0;
    scoreMultiplier = 1.0f;
    timer = 60;
    bubbleSpeed = 10.0f;

    const std::string file = pathOf("config.txt");
    std::ifstream cfg(file);
    if (!cfg)
    {
        std::cerr << "[CONFIG] No se encontró " << file << "; usando valores seguros.\n";
        return;
    }

    std::string line;
    while (std::getline(cfg, line))
    {
        auto trim = [](std::string s)
        {
            const auto first = s.find_first_not_of(" \t\r\n");
            const auto last = s.find_last_not_of(" \t\r\n");
            if (first == std::string::npos)
                return std::string{};
            return s.substr(first, last - first + 1);
        };

        line = trim(line);
        if (line.rfind("level:", 0) == 0)
        {
            int value = 1;
            std::sscanf(line.c_str(), "level: %d", &value);
            level = static_cast<Uint32>(std::clamp(value, 1, NUM_LEVELS_IMPLEMENTED));
        }
        else if (line.rfind("score:", 0) == 0)
        {
            int value = 0;
            std::sscanf(line.c_str(), "score: %d", &value);
            score = static_cast<Uint32>(std::max(0, value));
        }
        else if (line.rfind("score multiplier:", 0) == 0)
        {
            float value = 1.0f;
            std::sscanf(line.c_str(), "score multiplier: %fx", &value);
            scoreMultiplier = std::clamp(value, 0.1f, 100.0f);
        }
        else if (line.rfind("timer:", 0) == 0)
        {
            int value = 60;
            std::sscanf(line.c_str(), "timer: %d", &value);
            timer = std::max(1, value);
        }
        else if (line.rfind("bubble speed:", 0) == 0)
        {
            float value = 5.0f;
            std::sscanf(line.c_str(), "bubble speed: %f", &value);
            bubbleSpeed = std::clamp(value, 0.5f, 30.0f);
        }
    }

    // El formato original guardaba la velocidad multiplicada por 100.
    if (bubbleSpeed > 30.0f)
        bubbleSpeed *= 0.01f;
}

void Game::InitMap()
{
    for (auto& row : bubbles)
        for (Bubble*& bubble : row)
        {
            delete bubble;
            bubble = nullptr;
        }

    int selectedLevel = static_cast<int>(level);
    if (selectedLevel < 1 || selectedLevel > NUM_LEVELS_IMPLEMENTED)
        selectedLevel = 1;

    levelOffset = selectedLevel;
    numBubblesLeft = LevelManager::getInstance()->GetNumBubbles(selectedLevel);
    LevelManager::getInstance()->SetLevel(selectedLevel, bubbles);

    SDL_FRect cannonRect{
        static_cast<float>(CANNON_BUBBLE_OFFSET_X),
        static_cast<float>(CANNON_BUBBLE_OFFSET_Y),
        static_cast<float>(CANNON_BUBBLE_WIDTH),
        static_cast<float>(CANNON_BUBBLE_HEIGHT)
    };

    SDL_FRect spareRect{
        static_cast<float>(SPARE_BUBBLE_OFFSET_X),
        static_cast<float>(SPARE_BUFFLE_OFFSET_Y),
        static_cast<float>(SPARE_BUBBLE_W),
        static_cast<float>(SPARE_BUBBLE_H)
    };

    cannonBubble = new Bubble(RandomBubble(), cannonRect, true);
    spareBubble = new Bubble(RandomBubble(), spareRect, true);
    shotBubble = new Bubble(BUBBLE_NONE, cannonRect, false);

    cannonPoint.x = static_cast<int>(cannonRect.x + cannonRect.w * 0.5f);
    cannonPoint.y = static_cast<int>(cannonRect.y + cannonRect.h * 0.5f);
    startPoint = cannonPoint;
}

BubbleColor Game::RandomBubble()
{
    std::array<int, NUM_BUBBLES - 1> frequency{};

    for (const auto& row : bubbles)
    {
        for (const Bubble* bubble : row)
        {
            if (!bubble)
                continue;
            const BubbleColor color = bubble->GetColor();
            if (color >= BUBBLE_BLUE && color <= BUBBLE_RED)
                ++frequency[static_cast<int>(color)];
        }
    }

    std::vector<int> available;
    for (int i = 0; i < NUM_BUBBLES - 1; ++i)
        if (frequency[i] > 0)
            available.push_back(i);

    if (available.empty())
        return randomPlayableColor();

    const int index = available[std::uniform_int_distribution<int>(0, static_cast<int>(available.size()) - 1)(g_rng)];
    return static_cast<BubbleColor>(index);
}

bool Game::InsideRect(float pointX, float pointY, SDL_FRect& rect)
{
    return pointX >= rect.x && pointX <= rect.x + rect.w &&
           pointY >= rect.y && pointY <= rect.y + rect.h;
}

bool Game::InsideRect(int pointX, int pointY, SDL_Rect& rect)
{
    return pointInRect(pointX, pointY, rect);
}

Game::~Game()
{
    for (auto& row : bubbles)
        for (Bubble*& bubble : row)
        {
            delete bubble;
            bubble = nullptr;
        }

    delete cannonBubble;
    delete spareBubble;
    delete shotBubble;

    cannonBubble = nullptr;
    spareBubble = nullptr;
    shotBubble = nullptr;
}

void Game::Play()
{
    Uint64 previousCounter = SDL_GetPerformanceCounter();
    double accumulator = 0.0;

    constexpr double fixedDt = 1.0 / 60.0;
    constexpr double maxAccumulator = 0.20;

    while (running && g_running)
    {
        const Uint64 nowCounter = SDL_GetPerformanceCounter();
        double frameDt = static_cast<double>(nowCounter - previousCounter) /
                         static_cast<double>(SDL_GetPerformanceFrequency());
        previousCounter = nowCounter;
        frameDt = std::clamp(frameDt, 0.0, MAX_DT);
        accumulator = std::min(accumulator + frameDt, maxAccumulator);

        HandleEvents();

        while (accumulator >= fixedDt && !paused)
        {
            if (!frozen)
                ++currentFrame;
            else
                ++frozenFrame;

            Update();
            accumulator -= fixedDt;
        }

        Render();
        SDL_Delay(1);
    }

    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);
    SDL_RenderPresent(g_renderer);
}

void Game::Normalize(SDL_FPoint& vec, int length)
{
    const float norm = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    if (norm < 0.0001f)
    {
        vec.x = 0.0f;
        vec.y = -static_cast<float>(length);
        return;
    }

    const float scale = static_cast<float>(length) / norm;
    vec.x *= scale;
    vec.y *= scale;
}

void Game::ShootBubbleInit()
{
    if (!cannonBubble || !spareBubble || !shotBubble)
        return;

    const float dx = static_cast<float>(mousePoint.x - cannonPoint.x);
    const float dy = static_cast<float>(mousePoint.y - cannonPoint.y);
    const float len = std::sqrt(dx * dx + dy * dy);
    if (len < MIN_SHOT_LENGTH)
        return;

    SDL_FRect shotRect = cannonBubble->GetDestRect();
    shotRect.w = BUBBLE_WIDTH;
    shotRect.h = BUBBLE_HEIGHT;
    shotRect.x = cannonPoint.x - shotRect.w * 0.5f;
    shotRect.y = cannonPoint.y - shotRect.h * 0.5f - 60.0f;

    shotBubble->SetColor(cannonBubble->GetColor());
    shotBubble->SetTexture(cannonBubble->GetTexture());
    shotBubble->SetDestRect(shotRect);
    shotBubble->SetVisible(true);

    shotBubbleDirectionVector.x = dx / len;
    shotBubbleDirectionVector.y = dy / len;

    cannonBubble->SetColor(spareBubble->GetColor());
    cannonBubble->SetTexture(spareBubble->GetTexture());

    const BubbleColor next = RandomBubble();
    spareBubble->SetColor(next);
    spareBubble->SetTexture(TextureManager::getInstance()->GetBubble(next));

    startPoint.x = cannonPoint.x;
    startPoint.y = cannonPoint.y;
    t = 0.0f;
    shootingBubble = true;
}

void Game::HandleCollision(float& x, float& y, Bubble* bubble)
{
    if (!shotBubble)
        return;

    // Busca la celda vacía más cercana al punto de impacto. El código original
    // recorría todo el tablero y podía no encontrar una celda válida.
    // En el techo se usa exactamente la misma lógica: la celda superior más
    // cercana recibe la burbuja.
    int bestI = -1;
    int bestJ = -1;
    float bestDistance = std::numeric_limits<float>::max();

    for (int i = 0; i < BOARD_SIZE_X; ++i)
    {
        for (int j = 0; j < BOARD_SIZE_Y; ++j)
        {
            Bubble* cell = bubbles[i][j];
            if (!cell || cell->GetColor() != BUBBLE_NONE)
                continue;

            SDL_FRect r = cell->GetDestRect();
            const float cx = r.x + r.w * 0.5f;
            const float cy = r.y + r.h * 0.5f;
            const float dx = cx - x;
            const float dy = cy - y;
            const float d2 = dx * dx + dy * dy;

            if (d2 < bestDistance)
            {
                bestDistance = d2;
                bestI = i;
                bestJ = j;
            }
        }
    }

    if (bestI < 0 || bestJ < 0)
    {
        shootingBubble = false;
        shotBubble->SetVisible(false);
        t = 0.0f;
        return;
    }

    // Si la mejor celda está demasiado lejos, sigue retrocediendo la trayectoria
    // unos pasos hasta acercarse al grupo. Esto evita insertar una burbuja lejos.
    const float maxAttachDistance = static_cast<float>(BUBBLE_WIDTH * 4.0f);
    if (std::sqrt(bestDistance) > maxAttachDistance)
    {
        shootingBubble = false;
        shotBubble->SetVisible(false);
        t = 0.0f;
        return;
    }

    Bubble* target = bubbles[bestI][bestJ];
    target->SetColor(shotBubble->GetColor());
    target->SetTexture(shotBubble->GetTexture());
    target->SetVisible(true);

    Matchfinder::getInstance()->SetMatrix(bestI, bestJ, target->GetColor());

    int currentMoveScore = 0;
    const bool matched = Matchfinder::getInstance()->CheckForMatch(bestI, bestJ);

    if (matched)
    {
        if (consecutiveStrikes >= 5)
        {
            if ((strikesAfter5Consecutive & 3) < 2)
                SoundManager::getInstance()->PlaySound(SOUND_COMBO_BONUS1);
            else
                SoundManager::getInstance()->PlaySound(SOUND_COMBO_BONUS2);
            ++strikesAfter5Consecutive;
        }
        else
        {
            SoundManager::getInstance()->PlaySound(SOUND_COMBO);
        }

        Matchfinder::getInstance()->ClearPath(bubbles, &currentMoveScore, &numBubblesLeft);
        score += static_cast<Uint32>(std::max(0.0f, scoreMultiplier * currentMoveScore));

        if (consecutiveStrikes < 5)
        {
            ++consecutiveStrikes;
            if (consecutiveStrikes == 5)
                SoundManager::getInstance()->PlaySound(SOUND_FULLY_LOADED);
        }

        consecutiveStrikesRect.h = std::min(125, consecutiveStrikes * STRIKES_RECT_H);
        scoreMultiplier = consecutiveStrikes >= 5 ? 1.5f : 1.25f;
    }
    else
    {
        if (consecutiveStrikes >= 5)
            SoundManager::getInstance()->PlaySound(SOUND_RESET_BONUS);
        else
            SoundManager::getInstance()->PlaySound(SOUND_FAILED_SHOT);

        consecutiveStrikes = 0;
        strikesAfter5Consecutive = 0;
        consecutiveStrikesRect.h = 0;
        scoreMultiplier = 1.0f;
    }

    if (numBubblesLeft > 0)
    {
        const BubbleColor next = RandomBubble();
        spareBubble->SetColor(next);
        spareBubble->SetTexture(TextureManager::getInstance()->GetBubble(next));
    }

    shootingBubble = false;
    shotBubble->SetVisible(false);
    t = 0.0f;
}

void Game::ShootBubble()
{
    if (!shotBubble || !shotBubble->IsVisible())
    {
        shootingBubble = false;
        return;
    }

    float x = startPoint.x + shotBubbleDirectionVector.x * t;
    float y = startPoint.y + shotBubbleDirectionVector.y * t;

    SDL_FRect rect = shotBubble->GetDestRect();
    rect.x = x - rect.w * 0.5f;
    rect.y = y - rect.h * 0.5f;
    shotBubble->SetDestRect(rect);

    bool bounced = false;

    if (x >= WALL_RIGHT_X)
    {
        x = static_cast<float>(WALL_RIGHT_X);
        shotBubbleDirectionVector.x = -std::abs(shotBubbleDirectionVector.x);
        bounced = true;
    }
    else if (x <= WALL_LEFT_X)
    {
        x = static_cast<float>(WALL_LEFT_X);
        shotBubbleDirectionVector.x = std::abs(shotBubbleDirectionVector.x);
        bounced = true;
    }

    if (y <= WALL_TOP_Y)
    {
        y = static_cast<float>(WALL_TOP_Y);
        shotBubbleDirectionVector.y = std::abs(shotBubbleDirectionVector.y);
        bounced = true;
    }

    if (bounced)
    {
        startPoint.x = static_cast<int>(x);
        startPoint.y = static_cast<int>(y);
        t = 0.0f;
        SoundManager::getInstance()->PlaySound(SOUND_HIT_WALL);
    }

    // Colisión circular con las burbujas ocupadas.
    for (auto& row : bubbles)
    {
        for (Bubble* bubble : row)
        {
            if (!bubble || bubble->GetColor() == BUBBLE_NONE)
                continue;

            SDL_FRect br = bubble->GetDestRect();
            const float cx = br.x + br.w * 0.5f;
            const float cy = br.y + br.h * 0.5f;
            const float dx = x - cx;
            const float dy = y - cy;
            const float radius = (br.w + shotBubble->GetDestRect().w) * 0.46f;

            if (dx * dx + dy * dy <= radius * radius)
            {
                HandleCollision(x, y, bubble);
                return;
            }
        }
    }

    // Si alcanzó el techo se intenta pegar en la fila superior.
    if (y <= WALL_TOP_Y + 2)
    {
        HandleCollision(x, y, nullptr);
        return;
    }

    t += std::max(0.5f, bubbleSpeed);

    if (t > 2000.0f)
    {
        shootingBubble = false;
        shotBubble->SetVisible(false);
        t = 0.0f;
    }
}

void Game::RotateBubbles(RotateDirection direction)
{
    if (frozen)
        return;

    const Bubble* centerBubble = bubbles[LVL1_CENTER_X][LVL1_CENTER_Y];
    if (!centerBubble)
        return;

    const SDL_FRect centerRect = centerBubble->GetDestRect();
    const float cx = centerRect.x;
    const float cy = centerRect.y;

    const double sign = direction == ROTATE_LEFT ? -1.0 : 1.0;
    const double angle = sign * std::max(0.001, velocity);
    const float c = static_cast<float>(std::cos(angle));
    const float s = static_cast<float>(std::sin(angle));

    for (auto& row : bubbles)
    {
        for (Bubble* bubble : row)
        {
            if (!bubble)
                continue;

            SDL_FRect rect = bubble->GetDestRect();
            const float dx = rect.x - cx;
            const float dy = rect.y - cy;
            rect.x = cx + dx * c - dy * s;
            rect.y = cy + dx * s + dy * c;
            bubble->SetDestRect(rect);
        }
    }

    velocity = std::max(0.0, velocity - acceleration);
    if (velocity <= 0.0)
    {
        rotateLeft = false;
        rotateRight = false;
    }
}

void Game::Update()
{
    if (rotateLeft)
        RotateBubbles(ROTATE_LEFT);
    else if (rotateRight)
        RotateBubbles(ROTATE_RIGHT);

    if (shouldShootBubble && !shootingBubble)
    {
        ShootBubbleInit();
        shouldShootBubble = false;
    }

    if (shootingBubble)
        ShootBubble();

    if (numBubblesLeft <= 0 && !frozen)
    {
        SoundManager::getInstance()->PlaySound(SOUND_LEVEL_UP);
        ChangeLevel(1);
    }

    if (frozen)
    {
        if (frozenFrame >= 30)
        {
            frozen = false;
            frozenFrame = 0;
            InitMap();
        }
        return;
    }

    if (currentFrame % 60 == 0 && timer > 0)
        --timer;

    if (!g_normalMode && timer <= 0)
        running = false;
}

void Game::ChangeLevel(int offset)
{
    if (offset == 0)
        return;

    const int next = static_cast<int>(level) + offset;
    if (next < 1 || next > NUM_LEVELS_IMPLEMENTED)
    {
        if (next > NUM_LEVELS_IMPLEMENTED)
            running = false;
        return;
    }

    level = static_cast<Uint32>(next);
    levelOffset = next;
    frozen = true;
    frozenFrame = 0;
    shouldShootBubble = false;
    shootingBubble = false;
    t = 0.0f;
}

void Game::HandleMouseMotion(SDL_Event& event)
{
    mousePoint.x = event.motion.x;
    mousePoint.y = event.motion.y;
}

void Game::HandleMouseDown(SDL_Event& event)
{
    if (event.button.button != SDL_BUTTON_LEFT)
        return;

    mousePoint.x = event.button.x;
    mousePoint.y = event.button.y;

    if (!shootingBubble && !paused)
        shouldShootBubble = true;
}

void Game::HandleMouseUp(SDL_Event& event)
{
    if (event.button.button == SDL_BUTTON_LEFT)
    {
        mousePoint.x = event.button.x;
        mousePoint.y = event.button.y;
    }
}

void Game::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                running = false;
                g_running = false;
                break;

            case SDL_MOUSEMOTION:
                HandleMouseMotion(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    const int x = event.button.x;
                    const int y = event.button.y;

                    if (pointInRect(x, y, SDL_Rect{915, 80, 220, 230}))
                    {
                        running = false;
                        break;
                    }

                    if (pointInRect(x, y, SDL_Rect{930, 60, 190, 70}))
                    {
                        paused = !paused;
                        break;
                    }
                }

                HandleMouseDown(event);
                break;
            }

            case SDL_MOUSEBUTTONUP:
                HandleMouseUp(event);
                break;

            case SDL_KEYDOWN:
                if (event.key.repeat)
                    break;

                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        running = false;
                        break;

                    case SDLK_SPACE:
                        if (!shootingBubble && cannonBubble && spareBubble)
                        {
                            const SDL_FRect cannonRect = cannonBubble->GetDestRect();
                            const SDL_FRect spareRect = spareBubble->GetDestRect();
                            cannonBubble->SetDestRect(spareRect);
                            spareBubble->SetDestRect(cannonRect);

                            std::swap(cannonBubble, spareBubble);
                        }
                        break;

                    case SDLK_p:
                        paused = !paused;
                        break;

                    case SDLK_r:
                        Entity::ToggleRectangleVisible();
                        break;

                    case SDLK_LEFT:
                        if (!shootingBubble)
                            ChangeLevel(-1);
                        break;

                    case SDLK_l:
                        if (!shootingBubble)
                            ChangeLevel(1);
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

void Game::DisplayBackground()
{
    SDL_Renderer* renderer = GUIManager::getInstance()->GetRenderer();
    if (!renderer)
        return;

    SDL_Texture* background = TextureManager::getInstance()->GetBackground();
    if (background)
        SDL_RenderCopy(renderer, background, nullptr, nullptr);
    else
    {
        SDL_SetRenderDrawColor(renderer, 35, 35, 45, 255);
        SDL_RenderClear(renderer);
    }

    draw(renderer, pause_dokme_png, g_rect.pause, 930, 60, 190, 70);
    draw(renderer, cheat_png, g_rect.cheat, 900, 250, 320, 350);
    draw(renderer, quit_dokme_png, g_rect.quit, 915, 80, 220, 230);
}

void Game::DisplayScore()
{
    char text[32];
    std::snprintf(text, sizeof(text), "%u", score);
    FontRenderer::getInstance()->DisplayText(text, SCORE_OFFSET_X, SCORE_OFFSET_Y);
}

void Game::DisplayLevel()
{
    char text[32];
    std::snprintf(text, sizeof(text), "%u", level);
    FontRenderer::getInstance()->DisplayText(text, LEVEL_OFFSET_X, LEVEL_OFFSET_Y);
}

void Game::DisplayTimer()
{
    char text[32];
    const int safeTimer = std::max(0, timer);
    std::snprintf(text, sizeof(text), "%02d:%02d", safeTimer / 60, safeTimer % 60);
    FontRenderer::getInstance()->DisplayText(text, TIMER_OFFSET_X, TIMER_OFFSET_Y);
}

void Game::DisplayConsecutiveStrikesRect()
{
    SDL_Renderer* renderer = GUIManager::getInstance()->GetRenderer();
    if (!renderer || consecutiveStrikesRect.h <= 0)
        return;

    SDL_SetRenderDrawColor(renderer, 210, 78, 170, 255);
    SDL_RenderFillRect(renderer, &consecutiveStrikesRect);
}

void Game::DisplayBubbles()
{
    for (auto& row : bubbles)
        for (Bubble* bubble : row)
            if (bubble)
                bubble->Render();

    if (cannonBubble)
        cannonBubble->Render();
    if (spareBubble)
        spareBubble->Render();
    if (shotBubble && shotBubble->IsVisible())
        shotBubble->Render();
}

void Game::DisplayCannonToBubbleVector()
{
    SDL_Renderer* renderer = GUIManager::getInstance()->GetRenderer();
    if (!renderer || shootingBubble)
        return;

    SDL_SetRenderDrawColor(renderer, 70, 100, 255, 150);
    SDL_RenderDrawLine(renderer,
                       cannonPoint.x,
                       cannonPoint.y,
                       mousePoint.x,
                       mousePoint.y);
}

void Game::handle_canon()
{
    if (!canon)
        return;

    SDL_Renderer* renderer = GUIManager::getInstance()->GetRenderer();
    if (!renderer)
        return;

    const double angle = std::atan2(
        static_cast<double>(mousePoint.y - cannonPoint.y),
        static_cast<double>(mousePoint.x - cannonPoint.x));

    draw_image_zaviedar(renderer, canon, angle);
}

void Game::draw_image_zaviedar(SDL_Renderer* renderer, SDL_Texture* texture, double angle)
{
    if (!renderer || !texture)
        return;

    SDL_Rect rect{
        cannonPoint.x - 65,
        cannonPoint.y - 65,
        130,
        130
    };

    const double degrees = angle * 180.0 / PI + 90.0;
    SDL_RenderCopyEx(renderer, texture, nullptr, &rect, degrees, nullptr, SDL_FLIP_NONE);
}

void Game::Render()
{
    SDL_Renderer* renderer = GUIManager::getInstance()->GetRenderer();
    if (!renderer)
        return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    DisplayBackground();

    if (!g_normalMode)
        DisplayTimer();

    DisplayScore();
    DisplayLevel();
    DisplayConsecutiveStrikesRect();
    DisplayBubbles();
    DisplayCannonToBubbleVector();
    handle_canon();

    if (paused)
        renderText(renderer, "PAUSED - press P", 470, 360, 340, 70);

    SDL_RenderPresent(renderer);
}

// -----------------------------------------------------------------------------
// Funciones antiguas conservadas como wrappers compatibles con functions.h
// -----------------------------------------------------------------------------

void draw_image_zaviedar(SDL_Renderer* renderer, SDL_Texture* texture, double angle)
{
    if (!renderer || !texture)
        return;

    constexpr int IMAGE_WIDTH = 130;
    constexpr int IMAGE_HEIGHT = 130;

    SDL_Rect rect{
        (WINDOW_WIDTH - IMAGE_WIDTH) / 2 + 33,
        700 - IMAGE_HEIGHT - 33,
        IMAGE_WIDTH,
        IMAGE_HEIGHT
    };
    SDL_RenderCopyEx(renderer, texture, nullptr, &rect,
                     angle * 180.0 / PI - 30.0, nullptr, SDL_FLIP_NONE);
}

void draw_rect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color)
{
    if (!renderer)
        return;
    SDL_Rect rect{x, y, w, h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void draw_rect_zaviedar(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color)
{
    draw_rect(renderer, x, y, w, h, color);
}

void drawLineToMouseAndReflect(SDL_Renderer* renderer)
{
    if (!renderer)
        return;

    int mx = 0;
    int my = 0;
    SDL_GetMouseState(&mx, &my);

    const float sx = static_cast<float>(CANNON_BUBBLE_OFFSET_X + CANNON_BUBBLE_WIDTH / 2);
    const float sy = static_cast<float>(CANNON_BUBBLE_OFFSET_Y + CANNON_BUBBLE_HEIGHT / 2);
    float dx = static_cast<float>(mx) - sx;
    float dy = static_cast<float>(my) - sy;

    const float len = std::sqrt(dx * dx + dy * dy);
    if (len < 1.0f)
        return;

    dx /= len;
    dy /= len;

    // Solo una guía visual. No hace SDL_RenderPresent(); Render() es el único
    // responsable de presentar el frame.
    SDL_SetRenderDrawColor(renderer, 80, 120, 255, 130);
    SDL_RenderDrawLine(renderer,
                       static_cast<int>(sx),
                       static_cast<int>(sy),
                       mx,
                       my);
}

void canon_pain()
{
    // Compatibilidad con el código original. La representación real está en Game.
}

void handle_canon()
{
    // Compatibilidad con el código original.
}

void game(SDL_Renderer* /*renderer*/, bool /*day*/)
{
    Game gameInstance("Bubble Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                      WINDOW_WIDTH, WINDOW_HEIGHT, false);
    gameInstance.Play();
}

void shift_layers_3d(std::vector<std::vector<std::vector<int>>>& vec)
{
    if (vec.size() < 2)
        return;
    for (std::size_t i = vec.size() - 1; i > 1; --i)
        vec[i] = vec[i - 1];
}

void shift_layers_3d_ar(int arr[7][18][18])
{
    if (!arr)
        return;
    for (int i = 6; i > 1; --i)
        for (int x = 0; x < 18; ++x)
            for (int y = 0; y < 18; ++y)
                arr[i][x][y] = arr[i - 1][x][y];
}

void shiftBubbles(std::vector<std::vector<int>>& board, bool shouldShootBubble)
{
    if (!shouldShootBubble || board.size() < 2)
        return;
    for (std::size_t i = board.size() - 1; i > 1; --i)
        board[i] = board[i - 1];
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Permite arrancar desde el directorio del proyecto, build/, o pasando
    // explícitamente la carpeta de assets como primer argumento.
    if (argc > 1 && fs::is_directory(argv[1]))
    {
        g_assetRoot = argv[1];
    }
    else
    {
        // La raíz del proyecto contiene:
        //   assets/
        //   data/
        //   docs/
        //
        // Intentamos primero el directorio actual y después
        // el directorio padre del ejecutable (bin/BubbleShooter).

        std::array<fs::path, 4> candidates = {
            fs::current_path(),
            fs::current_path() / "..",
            fs::current_path() / "../..",
            fs::path(".")
        };

#ifdef __linux__
        {
            std::error_code ec;
            const fs::path exePath = fs::read_symlink(
                "/proc/self/exe", ec);

            if (!ec && !exePath.empty())
            {
                const fs::path exeDir = exePath.parent_path();

                candidates[1] = exeDir;
                candidates[2] = exeDir.parent_path();
            }
        }
#endif

        for (const fs::path& candidateRaw : candidates)
        {
            std::error_code ec;

            const fs::path candidate =
                fs::weakly_canonical(candidateRaw, ec);

            if (ec)
                continue;

            const bool hasAssets =
                fs::is_directory(candidate / "assets", ec) &&
                fs::is_directory(candidate / "data", ec);

            if (hasAssets)
            {
                g_assetRoot = candidate.string();
                break;
            }
        }
    }

    std::cout << "[Bubble Shooter] Asset root: " << fs::absolute(g_assetRoot) << '\n';

    if (!initSDL())
    {
        cleanupAudio();
        cleanupSDL();
        return EXIT_FAILURE;
    }

    if (!createMainWindow())
    {
        cleanupAudio();
        cleanupSDL();
        return EXIT_FAILURE;
    }

    setupRects();
    loadGlobalTextures();

    // Font de menú: se carga una sola vez.
    const std::string menuFontPath = findExisting({
        "Sans.ttf",
        "ARIAL.TTF",
        "arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    });

    if (!menuFontPath.empty())
    {
        g_menuFont = TTF_OpenFont(menuFontPath.c_str(), 24);
        if (!g_menuFont)
            logTTFError(menuFontPath.c_str());
    }

    // Los managers se crean una vez por aplicación, no una vez por partida.
    GUIManager::getInstance();
    TextureManager::getInstance();
    FontRenderer::getInstance();
    LevelManager::getInstance();
    Matchfinder::getInstance();
    SoundManager::getInstance();

    playMusic(g_musicChanged ? "paszamine2.mp3" : "paszamine.mp3");

    SDL_ShowCursor(SDL_ENABLE);

    while (g_running)
    {
        menu(g_renderer, !g_darkMode);
        SDL_RenderPresent(g_renderer);

        bool menuAction = true;
        while (menuAction && g_running)
        {
            SDL_Event event;
            bool gotEvent = false;

            while (SDL_PollEvent(&event))
            {
                gotEvent = true;

                if (event.type == SDL_QUIT)
                {
                    g_running = false;
                    break;
                }

                if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
                    continue;

                const int x = event.button.x;
                const int y = event.button.y;

                if (is_clicked(x, y, 70, 340, 150, 240))
                {
                    g_darkMode = !g_darkMode;
                    menu(g_renderer, !g_darkMode);
                    SDL_RenderPresent(g_renderer);
                }
                else if (is_clicked(x, y, 70, 100, 150, 240))
                {
                    g_running = false;
                    menuAction = false;
                }
                else if (is_clicked(x, y, 70, 220, 150, 240))
                {
                    g_setting = true;
                    drawSettings(g_renderer, !g_darkMode);
                    SDL_RenderPresent(g_renderer);

                    while (g_setting && g_running)
                    {
                        setting_ma_actions(g_renderer);
                        drawSettings(g_renderer, !g_darkMode);
                        SDL_RenderPresent(g_renderer);
                        SDL_Delay(4);
                    }

                    menu(g_renderer, !g_darkMode);
                    SDL_RenderPresent(g_renderer);
                }
                else if (is_clicked(x, y, 520, 390, 240, 300))
                {
                    g_startGame = false;
                    g_registerMenu = false;
                    g_menuRun = true;
                    menu_run(g_renderer, !g_darkMode);
                    SDL_RenderPresent(g_renderer);

                    while (g_menuRun && g_running)
                    {
                        menu_run_actions(g_renderer);
                        menu_run(g_renderer, !g_darkMode);
                        SDL_RenderPresent(g_renderer);
                        SDL_Delay(4);
                    }

                    if (g_registerMenu && g_running)
                    {
                        user_input();
                    }

                    if (g_running && g_startGame && !g_registerMenu && !g_menuRun)
                    {
                        Game currentGame("Bubble Shooter",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         WINDOW_WIDTH,
                                         WINDOW_HEIGHT,
                                         false);
                        currentGame.Play();
                    }

                    menuAction = false;
                }
            }

            if (!gotEvent)
                SDL_Delay(8);
        }
    }

    // Destrucción en orden inverso a la dependencia.
    // Los objetos Game ya fueron destruidos al salir de sus scopes.
    delete SoundManager::getInstance();
    delete Matchfinder::getInstance();
    delete LevelManager::getInstance();
    delete FontRenderer::getInstance();
    delete TextureManager::getInstance();
    delete GUIManager::getInstance();

    cleanupAudio();
    cleanupSDL();

    return EXIT_SUCCESS;
}
