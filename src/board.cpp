#include "board.h"

Board::Board() {
    init();
}

void Board::init() {
    // Empty all squares
    for (int row = 0;row < BOARD_SIZE;row ++) {
        for (int col = 0;col < BOARD_SIZE;col ++) {
            board[row][col] = {EMPTY, NONE};
        }
    }

    // Pawns
    for (int col = 0;col < BOARD_SIZE;col ++) {
        board[1][col] = {PAWN, BLACK};
        board[6][col] = {PAWN, WHITE};
    }

    // Rooks
    board[0][0] = board[0][7] = {ROOK, BLACK};
    board[7][0] = board[7][7] = {ROOK, WHITE};

    // Knights
    board[0][1] = board[0][6] = {KNIGHT, BLACK};
    board[7][1] = board[7][6] = {KNIGHT, WHITE};

    // Bishops
    board[0][2] = board[0][5] = {BISHOP, BLACK};
    board[7][2] = board[7][5] = {BISHOP, WHITE};

    // Queens
    board[0][3] = {QUEEN, BLACK};
    board[7][3] = {QUEEN, WHITE};

    // Kings
    board[0][4] = {KING, BLACK};
    board[7][4] = {KING, WHITE};
}

bool Board::isSquareValid(int row, int col) const {
    return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

Piece Board::getPiece(int row, int col) const {
    if (!isSquareValid(row, col)) return {EMPTY, NONE};
    return board[row][col];
}

void Board::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isSquareValid(fromRow, fromCol) || !isSquareValid(toRow, toCol)) return;

    turn = (turn == WHITE) ? BLACK : WHITE;
    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = {EMPTY, NONE};
}

bool Board::isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    if (!isSquareValid(fromRow, fromCol) || !isSquareValid(toRow, toCol)) return false;

    Piece fromPiece = board[fromRow][fromCol];

    if (turn == fromPiece.color) {
        return true;
    } else {
        return false;
    }
}

