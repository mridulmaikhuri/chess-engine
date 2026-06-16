#include <SDL3/SDL.h>
#include <iostream>
#include <SDL3_image/SDL_image.h>
#include "board.h"
using namespace std;

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800;
const int TILE_SIZE = WINDOW_WIDTH/BOARD_SIZE;

enum class GameScreen {
    PLAYING,
    PROMOTION,
    GAME_OVER
};

struct AppState {
    Board board;

    bool pieceSelected = false;
    int selectedRow = -1;
    int selectedCol = -1;

    vector<pair<int,int>> legalMoves;

    GameScreen screen = GameScreen::PLAYING;
    int promotionRow = -1;
    int promotionCol = -1;
    PieceColor promotionColor = NONE;
};


SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        cout << "Failed to load image\n";
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
        cout << "Failed to create texture\n";
    }

    return texture;
}

SDL_Texture* gTextures[7][3] = {};

void loadAllTextures(SDL_Renderer* renderer) {
    gTextures[PAWN][WHITE] = loadTexture(renderer, "../assets/white-pawn.png");
    gTextures[PAWN][BLACK] = loadTexture(renderer, "../assets/black-pawn.png");
    gTextures[ROOK][WHITE] = loadTexture(renderer, "../assets/white-rook.png");
    gTextures[ROOK][BLACK] = loadTexture(renderer, "../assets/black-rook.png");
    gTextures[KNIGHT][WHITE] = loadTexture(renderer, "../assets/white-knight.png");
    gTextures[KNIGHT][BLACK] = loadTexture(renderer, "../assets/black-knight.png");
    gTextures[BISHOP][WHITE] = loadTexture(renderer, "../assets/white-bishop.png");
    gTextures[BISHOP][BLACK] = loadTexture(renderer, "../assets/black-bishop.png");
    gTextures[QUEEN][WHITE] = loadTexture(renderer, "../assets/white-queen.png");
    gTextures[QUEEN][BLACK] = loadTexture(renderer, "../assets/black-queen.png");
    gTextures[KING][WHITE] = loadTexture(renderer, "../assets/white-king.png");
    gTextures[KING][BLACK] = loadTexture(renderer, "../assets/black-king.png");
}

void destroyAllTextures() {
    for (int t = 1;t < 7;t ++) {
        for (int c = 1;c < 3;c ++) {
            if (gTextures[t][c]) SDL_DestroyTexture(gTextures[t][c]);
        }
    }
}

vector<pair<int,int>> computeLegalMoves(Board& board, int fromRow, int fromCol) {
    vector<pair<int,int>> moves;
    for (int r = 0;r < BOARD_SIZE;r ++) {
        for (int c = 0;c < BOARD_SIZE;c ++) {
            if (board.isMoveValid(fromRow, fromCol, r, c))
                moves.push_back({r, c});
        }
    }
    return moves;
}

bool isLegalMoveTarget(const vector<pair<int,int>>& moves, int row, int col) {
    for (const auto& move : moves) {
        if (move.first == row && move.second == col) return true;
    }
    return false;
}

void handleBoardClick(AppState& state, int row, int col) {
    Board& board = state.board;

    if (!state.pieceSelected) {
        Piece p = board.getPiece(row, col);
        if (p.type != EMPTY && p.color == board.getTurn()) {
            state.pieceSelected = true;
            state.selectedRow = row;
            state.selectedCol = col;
            state.legalMoves = computeLegalMoves(board, row, col);
        }
    } else {
        Piece clicked = board.getPiece(row, col);

        if (clicked.type != EMPTY && clicked.color == board.getTurn()) {
            state.selectedRow = row;
            state.selectedCol = col;
            state.legalMoves = computeLegalMoves(board, row, col);
            return;
        }

        if (board.isMoveValid(state.selectedRow, state.selectedCol, row, col)) {
            Piece movingPiece = board.getPiece(state.selectedRow, state.selectedCol);

            bool isPromotion = (movingPiece.type == PAWN) 
            && ((movingPiece.color == WHITE && row == 0)
            || (movingPiece.color == BLACK && row == 7));

            if (isPromotion) {
                state.screen = GameScreen::PROMOTION;
                state.promotionRow = row;
                state.promotionCol = col;
                state.promotionColor = movingPiece.color;
            } else {
                board.makeMove(state.selectedRow, state.selectedCol, row, col, EMPTY);
                state.pieceSelected = false;
                state.selectedRow = -1;
                state.selectedCol = -1;
                state.legalMoves.clear();

                if (board.isGameOver()) {
                    state.screen = GameScreen::GAME_OVER;
                }
            }
        } else {
            state.pieceSelected = false;
            state.selectedRow = -1;
            state.selectedCol = -1;
            state.legalMoves.clear();
        }
    }
}

void handlePromotionClick(AppState& state, int mouseX, int mouseY, int tileSize) {
    const PieceType choices[4]  = { QUEEN, ROOK, BISHOP, KNIGHT };
    int totalW = 4 * tileSize;
    int startX = (BOARD_SIZE * tileSize - totalW) / 2;
    int startY = (BOARD_SIZE * tileSize - tileSize) / 2;

    for (int i = 0;i < 4;i ++) {
        SDL_FRect box = {
            static_cast<float>(startX + i * tileSize),
            static_cast<float>(startY),
            static_cast<float>(tileSize),
            static_cast<float>(tileSize)
        };

        if (mouseX >= box.x && mouseX < box.x + box.w &&
        mouseY >= box.y && mouseY < box.y + box.h && 
        state.board.isMoveValid(state.selectedRow, state.selectedCol,
        state.promotionRow, state.promotionCol)) {
            state.board.makeMove(
                state.selectedRow, state.selectedCol,
                state.promotionRow, state.promotionCol, 
                choices[i]
            );
            state.pieceSelected = false;
            state.selectedRow = -1;
            state.selectedCol = -1;
            state.legalMoves.clear();
            state.screen = state.board.isGameOver()
                ? GameScreen::GAME_OVER
                : GameScreen::PLAYING;
            return;
        }
    }
}


void renderBoard(SDL_Renderer* renderer, AppState& state, int tileSize) {
    Board& board = state.board;

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

            if (isLegalMoveTarget(state.legalMoves, row, col)) {
                Piece target = board.getPiece(row, col);
                if (target.type == EMPTY) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 60);
                    float cx = square.x + square.w / 2.0f;
                    float cy = square.y + square.h / 2.0f;
                    float r = tileSize * 0.15f;

                    for (float dy = -r; dy <= r;dy += 1.0f) {
                        float dx = sqrtf(r * r - dy * dy);
                        SDL_FRect dot = {cx - dx, cy + dy, 2 * dx, 1};
                        SDL_RenderFillRect(renderer, &dot);
                    }
                } else {
                    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 80);
                    SDL_RenderFillRect(renderer, &square);
                }
            }

            if (row == state.selectedRow && col == state.selectedCol) {
                SDL_SetRenderDrawColor(renderer, 50, 50, 200, 220);
                for (int i = 0;i < 3;i ++) {
                    SDL_FRect border = {
                        square.x + i, square.y + i,
                        square.w - 2.0f * i, square.h - 2.0f * i
                    };
                    SDL_RenderRect(renderer, &border);
                }
            }

            Piece piece = board.getPiece(row, col);
            if (piece.type == KING && board.isKingInCheck(piece.color)) {
                SDL_SetRenderDrawColor(renderer, 20, 30, 20, 120);
                SDL_RenderFillRect(renderer, &square);
            }
        }
    }

    for (int row = 0; row < BOARD_SIZE; row ++) {
        for (int col = 0; col < BOARD_SIZE; col ++) {
            Piece piece = board.getPiece(row, col);
            if (piece.type == EMPTY) continue;

            SDL_Texture* texture = gTextures[piece.type][piece.color];

            if (!texture) continue;

            SDL_FRect dst = {
                static_cast<float>(col * tileSize),
                static_cast<float>(row * tileSize),
                static_cast<float>(tileSize),
                static_cast<float>(tileSize)
            };

            SDL_RenderTexture(renderer, texture, nullptr, &dst);
        }
    }   
}

void renderPromotionDialog(SDL_Renderer* renderer, AppState& state, int tileSize) {
    const PieceType choices[4] = { QUEEN, ROOK, BISHOP, KNIGHT };
    int totalW = 4 * tileSize;
    int startX = (BOARD_SIZE * tileSize - totalW) / 2;
    int startY = (BOARD_SIZE * tileSize - tileSize) / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
    SDL_FRect overlay = { 0, 0,
    static_cast<float>(BOARD_SIZE * tileSize),
    static_cast<float>(BOARD_SIZE * tileSize)};

    SDL_RenderFillRect(renderer, &overlay);

    for (int i = 0;i < 4;i ++) {
        SDL_FRect box = {
            static_cast<float>(startX + i * tileSize),
            static_cast<float>(startY),
            static_cast<float>(tileSize),
            static_cast<float>(tileSize)
        };

        SDL_SetRenderDrawColor(renderer, 230, 230, 200, 255);
        SDL_RenderFillRect(renderer, &box);

        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderRect(renderer, &box);

        SDL_Texture* texture = gTextures[choices[i]][state.promotionColor];
        if (texture) SDL_RenderTexture(renderer, texture, nullptr, &box);
    }
}

void renderGameOver(SDL_Renderer* renderer, AppState& state, int tileSize) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_FRect overlay = {0, 0, 
    static_cast<float>(BOARD_SIZE * tileSize),
    static_cast<float>(BOARD_SIZE * tileSize)};

    SDL_RenderFillRect(renderer, &overlay);

    int bw = tileSize * 4, bh = tileSize;
    SDL_FRect banner = {
        static_cast<float>((BOARD_SIZE * tileSize - bw)/2),
        static_cast<float>((BOARD_SIZE * tileSize - bh)/2),
        static_cast<float>(bw),
        static_cast<float>(bh)
    };
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 230);
    SDL_RenderFillRect(renderer, &banner);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderRect(renderer, &banner);

    PieceColor winner = state.board.getWinner();
    if (winner != NONE) {
        SDL_Texture* texture = gTextures[KING][winner];
        if (texture) {
            SDL_FRect icon = {
                banner.x + (bw - tileSize) / 2.0f,
                banner.y,
                static_cast<float>(tileSize),
                static_cast<float>(tileSize)
            };
            SDL_RenderTexture(renderer, texture, nullptr, &icon);
        }
    }
}

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

    if (!renderer) {
        cout << "Renderer Creation Failed\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    loadAllTextures(renderer);

    SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    SDL_SetCursor(cursor);

    AppState state;

    bool running = true;
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
                
                if (state.screen == GameScreen::PLAYING) {
                    handleBoardClick(state, row, col);
                } else if (state.screen == GameScreen::PROMOTION) {
                    handlePromotionClick(state, mouseX, mouseY, TILE_SIZE);
                } else if (state.screen == GameScreen::GAME_OVER) {
                    state.board.init();
                    state.pieceSelected = false;
                    state.selectedRow = -1;
                    state.selectedCol = -1;
                    state.legalMoves.clear();
                    state.screen = GameScreen::PLAYING;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderBoard(renderer, state, TILE_SIZE);

        if (state.screen == GameScreen::PROMOTION) {
            renderPromotionDialog(renderer, state, TILE_SIZE);
        } else if (state.screen == GameScreen::GAME_OVER) {
            renderGameOver(renderer, state, TILE_SIZE);
        }

        SDL_RenderPresent(renderer);
    }

    destroyAllTextures();
    SDL_DestroyCursor(cursor);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;   
}