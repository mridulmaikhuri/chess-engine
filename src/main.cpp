#include <SDL3/SDL.h>
#include <iostream>
using namespace std;

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800;
const int BOARD_SIZE = 8;
const int TILE_SIZE = WINDOW_WIDTH/BOARD_SIZE;

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL Init Failed\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Chess",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        cout << "Window Creation Failed\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int row = 0;row < BOARD_SIZE;row ++) {
            for (int col = 0;col < BOARD_SIZE;col ++) {
                bool isLightSquare = (row + col)%2 == 0;
                if (isLightSquare) {
                    SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);
                }

                SDL_FRect square = {
                    static_cast<float>(col * TILE_SIZE),
                    static_cast<float>(row * TILE_SIZE),
                    static_cast<float>(TILE_SIZE),
                    static_cast<float>(TILE_SIZE)  
                };

                SDL_RenderFillRect(renderer, &square);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;   
}