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
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);

    if (rowDiff != colDiff) return false;

    int rd = (fromRow < toRow) ? 1 : -1;
    int cd = (fromCol < toCol) ? 1 : -1;

    int row = fromRow, col = fromCol;

    for (int i = 0;i < rowDiff - 1;i ++) {
        row += rd, col += cd;
        Piece piece = board[row][col];
        if (piece.type != EMPTY) return false;
    }

    return true;
}

bool Board::isQueenMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    return isBishopMoveValid(fromRow, fromCol, toRow, toCol) || 
        isRookMoveValid(fromRow, fromCol, toRow, toCol);
}

bool Board::isKingMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);

    //TODO: Handle the king check event

    return max(rowDiff, colDiff) == 1;
}

bool Board::isKingInCheck(PieceColor color) const {
    int kingRow = -1, kingCol = -1;

    for (int row = 0;row < BOARD_SIZE;row ++) {
        for (int col = 0;col < BOARD_SIZE;col ++) {
            Piece piece = board[row][col];

            if (piece.type == KING && piece.color == color) {
                kingRow = row;
                kingCol = col;
                break;
            }
        }
    }

    if (kingRow == -1) {
        return false;
    }

    PieceColor enemyColor = (color == WHITE) ? BLACK : WHITE;

    for (int row = 0;row < BOARD_SIZE;row ++) {
        for (int col = 0;col < BOARD_SIZE;col ++) {
            Piece piece = board[row][col];

            if (piece.color != enemyColor) continue;

            switch (piece.type) {
                case PAWN:
                    int direction = (enemyColor == WHITE) ? -1 : 1;
                    if (row + direction == kingRow && (col - 1 == kingCol || col + 1 == kingCol)) return true;
                    break;
                
                case ROOK:
                    if (isRookMoveValid(row, col, kingRow, kingCol)) return true;
                    break;
                
                case KNIGHT:
                    if (isKnightMoveValid(row, col, kingRow, kingCol)) return true;
                    break;

                case BISHOP:
                    if (isBishopMoveValid(row, col, kingRow, kingCol)) return true;
                    break;

                case QUEEN:
                    if (isQueenMoveValid(row, col, kingRow, kingCol)) return true;
                    break;
                
                case KING:
                    int rowDiff = abs(row - kingRow);
                    int colDiff = abs(col - kingCol);

                    if (max(rowDiff, colDiff) == 1) return true;
                    break;
                
                default:
                    break;
            }
        }
    }

    return false;
}

bool Board::wouldLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol) const {
    Board temp = *this;

    Piece movingPiece = temp.board[fromRow][fromCol];

    temp.board[toRow][toCol] = movingPiece;
    temp.board[fromRow][fromCol] = {EMPTY, NONE};

    return temp.isKingInCheck(movingPiece.color);
}