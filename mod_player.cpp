#include <iostream>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BALL_SIZE = 20;
const int BALL_SPEED = 3;
const std::string MUSIC_DIRECTORY = "./music";
const std::string BACKGROUND_IMAGE = "background.bmp";

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
Mix_Music* music = nullptr;
std::vector<std::string> modFiles;
int currentSongIndex = 0;
std::string slowScrollText = "Coded by _SiCK the cat @ afflicted.sh using C++ and SDL - ever since i saw the pdx her collection demo player i wanted to make my own mod player... It's done now. Thanks to UpperEchelon for the GFX ... Add your music to the folder ./music.. in the cwd, play/stop is space. N is next. Q is quit! enjoy! greetz to d0cksey, jean lupin, maurice, rico, shino, chasse, boxswapper, and the rest of the homies - you know who you are. <3  ";
int scrollOffset = 0;

struct Ball {
    int x;
    int y;
    int velX;
    int velY;
};

SDL_Texture* backgroundTexture = nullptr;

void initializeSDL() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    Mix_Init(MIX_INIT_MOD); // Initialize SDL_mixer with support for MOD music
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // Open audio with default parameters
    window = SDL_CreateWindow("MOD Tracker Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("terminus.ttf", 20);
}

void loadMusicFiles() {
    DIR* dir;
    struct dirent* entry;
    dir = opendir(MUSIC_DIRECTORY.c_str());

    if (!dir) {
        std::cerr << "Error: Cannot open music directory.\n";
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG && std::strstr(entry->d_name, ".mod")) {
            modFiles.push_back(entry->d_name);
        }
    }

    closedir(dir);
}

void playMusic(const std::string& filename) {
    music = Mix_LoadMUS(filename.c_str());
    Mix_PlayMusic(music, -1); // Play the music on loop
}

void stopMusic() {
    Mix_HaltMusic();
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
}

void loadBackgroundImage() {
    SDL_Surface* surface = IMG_Load(BACKGROUND_IMAGE.c_str());
    if (!surface) {
        std::cerr << "Error: Failed to load background image.\n";
        return;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void drawBackground() {
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }
}

void drawSlowScrollText() {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, slowScrollText.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int textWidth = surface->w;
    int textHeight = surface->h;
    SDL_FreeSurface(surface);

    scrollOffset -= 1;
    if (scrollOffset < -textWidth) {
        scrollOffset = SCREEN_WIDTH;
    }

    SDL_Rect srcRect = {0, 0, textWidth, textHeight};
    SDL_Rect dstRect = {scrollOffset, SCREEN_HEIGHT - textHeight, textWidth, textHeight};
    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
    SDL_DestroyTexture(texture);
}

void updateBallPosition(Ball& ball) {
    ball.x += ball.velX;
    ball.y += ball.velY;

    // Handle collisions with window boundaries
    if (ball.x <= 0 || ball.x + BALL_SIZE >= SCREEN_WIDTH) {
        ball.velX = -ball.velX;
    }
    if (ball.y <= 0 || ball.y + BALL_SIZE >= SCREEN_HEIGHT) {
        ball.velY = -ball.velY;
    }
}

int main(int argc, char* argv[]) {
    initializeSDL();
    loadMusicFiles();
    loadBackgroundImage();

    Ball ball = {SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2, BALL_SPEED, BALL_SPEED};

    bool quit = false;
    SDL_Event event;

    Uint32 timer = SDL_GetTicks();
    Uint32 lastTime = timer;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (!modFiles.empty()) {
                            if (music && Mix_PlayingMusic()) {
                                stopMusic();
                            } else {
                                playMusic(MUSIC_DIRECTORY + "/" + modFiles[currentSongIndex]);
                            }
                        }
                        break;
                    case SDLK_n:
                        if (!modFiles.empty()) {
                            currentSongIndex = (currentSongIndex + 1) % modFiles.size();
                            stopMusic();
                            playMusic(MUSIC_DIRECTORY + "/" + modFiles[currentSongIndex]);
                        }
                        break;
                    case SDLK_q:
                        quit = true;
                        break;
                }
            }
        }

        // Slow scroll update
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime >= 20000) {
            lastTime = currentTime;
            scrollOffset += 1;
            if (scrollOffset >= SCREEN_WIDTH) {
                scrollOffset = 0;
            }
        }

        // Ball update
        updateBallPosition(ball);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawBackground();
        // Draw the ball
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect ballRect = {ball.x, ball.y, BALL_SIZE, BALL_SIZE};
        SDL_RenderFillRect(renderer, &ballRect);

        drawSlowScrollText();

        SDL_RenderPresent(renderer);
    }

    stopMusic();
    SDL_DestroyTexture(backgroundTexture);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    Mix_Quit(); // Quit SDL_mixer subsystem
    SDL_Quit();

    return 0;
}
