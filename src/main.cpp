#include <SDL3/SDL.h>
#include <iostream>
#include <SDL3_image/SDL_image.h>
#include "board.h"
using namespace std;

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800;
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

    auto drawPiece = [&](const char* path) -> SDL_Texture* {
        SDL_Surface* surface = IMG_Load(path);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        return texture;
    };

    SDL_Texture* whitePawn = drawPiece("../assets/white-pawn.png");
    SDL_Texture* blackPawn = drawPiece("../assets/black-pawn.png");
    SDL_Texture* whiteRook = drawPiece("../assets/white-rook.png");
    SDL_Texture* blackRook = drawPiece("../assets/black-rook.png");
    SDL_Texture* whiteKnight = drawPiece("../assets/white-knight.png");
    SDL_Texture* blackKnight = drawPiece("../assets/black-knight.png");
    SDL_Texture* whiteBishop = drawPiece("../assets/white-bishop.png");
    SDL_Texture* blackBishop = drawPiece("../assets/black-bishop.png");
    SDL_Texture* whiteQueen = drawPiece("../assets/white-queen.png");
    SDL_Texture* blackQueen = drawPiece("../assets/black-queen.png");
    SDL_Texture* whiteKing = drawPiece("../assets/white-king.png");
    SDL_Texture* blackKing = drawPiece("../assets/black-king.png");

    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));

    // Initialize board
    Board board;

    bool running = true;
    bool pieceSelected = false;
    int selectedRow = -1, selectedCol = -1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                int col = mouseX/TILE_SIZE;
                int row = mouseY/TILE_SIZE;
                //cout << row << " " << col << "\n";
                
                if (!pieceSelected) {
                    if (board.getPiece(row, col).type != EMPTY) {
                        pieceSelected = true;
                        selectedRow = row;
                        selectedCol = col;
                    }
                } else if (board.isMoveValid(selectedRow, selectedCol, row, col)) {
                    //cout << "Inside else if block\n";
                    board.movePiece(selectedRow, selectedCol, row, col);
                    pieceSelected = false;
                    selectedRow = -1;
                    selectedCol = -1;
                } else {
                    //printf("Inside else block\n");
                    pieceSelected = false;
                    selectedRow = -1;
                    selectedCol = -1;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw board squares
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
                
                // set boarder around selected square
                if (row == selectedRow && col == selectedCol) {
                    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
                    for (int i = 0;i < 3;i ++) {
                        SDL_FRect border = {
                            square.x + i,
                            square.y + i,
                            square.w - 2 * i,
                            square.h - 2 * i
                        };
                        SDL_RenderRect(renderer, &border);
                    }
                }
            }
        }

        // Draw Pieces
        for (int row = 0;row < BOARD_SIZE;row ++) {
            for (int col = 0;col < BOARD_SIZE;col ++) {
                Piece piece = board.getPiece(row, col);
                if (piece.type == EMPTY) continue;

                SDL_Texture* texture = nullptr;
                if (piece.type == PAWN && piece.color == WHITE) texture = whitePawn;
                else if (piece.type == PAWN && piece.color == BLACK) texture = blackPawn;
                else if (piece.type == ROOK && piece.color == WHITE) texture = whiteRook;
                else if (piece.type == ROOK && piece.color == BLACK) texture = blackRook;
                else if (piece.type == KNIGHT && piece.color == WHITE) texture = whiteKnight;
                else if (piece.type == KNIGHT && piece.color == BLACK) texture = blackKnight;
                else if (piece.type == BISHOP && piece.color == WHITE) texture = whiteBishop;
                else if (piece.type == BISHOP && piece.color == BLACK) texture = blackBishop;
                else if (piece.type == QUEEN && piece.color == WHITE) texture = whiteQueen;
                else if (piece.type == QUEEN && piece.color == BLACK) texture = blackQueen;
                else if (piece.type == KING && piece.color == WHITE) texture = whiteKing;
                else if (piece.type == KING && piece.color == BLACK) texture = blackKing;

                SDL_FRect dstRect = {
                    static_cast<float>(col * TILE_SIZE),
                    static_cast<float>(row * TILE_SIZE),
                    static_cast<float>(TILE_SIZE),
                    static_cast<float>(TILE_SIZE)
                };

                SDL_RenderTexture(renderer, texture, NULL, &dstRect);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;   
}