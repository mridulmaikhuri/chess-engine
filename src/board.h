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

    bool whiteKingMoved;
    bool blackKingMoved;

    bool whiteKingsideRookMoved;
    bool whiteQueensideRookMoved;

    bool blackKingsideRookMoved;
    bool blackQueensideRookeMoved;

    bool enPassantAvailable;
    int enPassantRow;
    int enPassantCol;

    bool gameOver;
    PieceColor winner;

    bool isSquareValid(int row, int col) const;

    bool wouldLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol) const;

    bool hasAnyLegalMove(PieceColor color);
public:
    Board();

    void init();

    Piece getPiece(int row, int col) const;

    void movePiece(int fromRow, int fromCol, int toRow, int toCol);

    bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol);

    bool isPawnMoveValid(int fromRow, int fromCol, int toRow, int toCol);

    bool isRookMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isKnightMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isBishopMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isKingMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isQueenMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isKingInCheck(PieceColor color) const;

    bool isCheckmate(PieceColor color);

    bool isStalemate(PieceColor color);

    bool isMoveLegal(int fromRow, int fromCol, int toRow, int toCol);

    void promotePawn(int row, int col, PieceType promotionPiece);
};

