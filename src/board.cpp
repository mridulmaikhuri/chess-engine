#include "board.h"
#include <iostream>
using namespace std;

Board::Board() {
    init();
    turn = WHITE;
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
    if (!isSquareValid(fromRow, fromCol) || 
        !isSquareValid(toRow, toCol) || 
        (fromRow == fromCol && toRow == toCol)) return false;

    Piece fromPiece = board[fromRow][fromCol];
    Piece toSquare = board[toRow][toCol];
    //cout << fromPiece.type << " " << fromPiece.color << "\n";

    if (turn == fromPiece.color && turn != toSquare.color) {
        if (fromPiece.type == PAWN) return isPawnMoveValid(fromRow, fromCol, toRow, toCol);
        if (fromPiece.type == ROOK) return isRookMoveValid(fromRow, fromCol, toRow, toCol);
        if (fromPiece.type == KNIGHT) return isKnightMoveValid(fromRow, fromCol, toRow, toCol);
        if (fromPiece.type == BISHOP) return isBishopMoveValid(fromRow, fromCol, toRow, toCol);
        if (fromPiece.type == QUEEN) return isQueenMoveValid(fromRow, fromCol, toRow, toCol);
        if (fromPiece.type == KING) return isKingMoveValid(fromRow, fromCol, toRow, toCol);
    } else {
        return false;
    }
}

bool Board::isPawnMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    return true;
}

bool Board::isRookMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);

    if (rowDiff != 0 && colDiff != 0) return false;

    if (rowDiff != 0 && colDiff == 0) {
        int stRow = min(fromRow, toRow);
        int enRow = max(fromRow, toRow);
        for (int row = stRow + 1;row < enRow;row ++) {
            Piece piece = board[row][fromCol];
            if (piece.type != EMPTY) return false;
        }
    } else if (rowDiff == 0 && colDiff != 0) {
        int stCol = min(fromCol, toCol);
        int enCol = max(fromCol, toCol);
        for (int col = stCol + 1;col < enCol;col ++) {
            Piece piece = board[fromRow][col];
            if (piece.type != EMPTY) return false;
        }
    }

    return true;
}

bool Board::isKnightMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);

    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool Board::isBishopMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    return true;
}

bool Board::isQueenMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    return true;
}

bool Board::isKingMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    return true;
}

