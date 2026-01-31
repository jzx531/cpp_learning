#include <ncurses.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <iostream>

int main() {
    // ===== ncurses 测试 =====
    initscr();
    printw("ncurses OK. Press any key...\n");
    refresh();
    getch();
    endwin();

    // ===== SDL 测试（不创建窗口）=====
    if (SDL_Init(0) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::cout << "SDL core init OK" << std::endl;

    // ===== SDL_mixer 测试 =====
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) != 0) {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL_mixer OK" << std::endl;

    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}
