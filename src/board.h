#pragma once

const int BOARD_SIZE = 8;

enum PieceType {
    EMPTY,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
};

enum PieceColor {
    NONE,
    WHITE,
    BLACK
};

struct Piece {
    PieceType type;
    PieceColor color;
};

class Board {
private:
    Piece board[BOARD_SIZE][BOARD_SIZE];
    PieceColor turn;

    bool isSquareValid(int row, int col) const;
    
public:
    Board();

    void init();

    Piece getPiece(int row, int col) const;

    void movePiece(int fromRow, int fromCol, int toRow, int toCol);

    bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isPawnMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isRookMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isKnightMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isBishopMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isKingMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isQueenMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;
};

