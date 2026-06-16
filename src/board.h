#pragma once
#include <cstdint>
#include <unordered_map>
using namespace std;

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

struct MoveRecord {
    int fromRow, fromCol, toRow, toCol;
    int fiftyMove;
    uint64_t prevHash;
    Piece movedPiece;
    Piece capturedPiece;
    PieceType promotedTo;

    bool prevEnPassantAvailable;
    int prevEnPassantRow, prevEnPassantCol;

    bool prevWhiteKingMoved;
    bool prevBlackKingMoved;
    bool prevWhiteKingSideRookMoved;
    bool prevWhiteQueenSideRookMoved;
    bool prevBlackKingSideRookMoved;
    bool prevBlackQueenSideRookMoved;

    bool wasEnPassant;
    int enPassantCapturedRow, enPassantCapturedCol;
    
    bool wasCastle;
    int rookFromCol;
    int rookToCol;
};

class Board {
private:
    Piece board[BOARD_SIZE][BOARD_SIZE];
    PieceColor turn;

    int fiftyMove;
    unordered_map<uint64_t, int> repTable;

    // Castling
    bool whiteKingMoved;
    bool blackKingMoved;
    bool whiteKingsideRookMoved;
    bool whiteQueensideRookMoved;
    bool blackKingsideRookMoved;
    bool blackQueensideRookeMoved;

    // en-passant
    bool enPassantAvailable;
    int enPassantRow;
    int enPassantCol;

    bool gameOver;
    PieceColor winner;

    int whiteKingRow, whiteKingCol;
    int blackKingRow, blackKingCol;

    uint64_t zobristPieces[12][64];
    uint64_t zobristturn;
    uint64_t zobristCastle[4];
    uint64_t zobristEnPassant[8];
    uint64_t currentHash;

    MoveRecord history[1024];
    int historySize;

    uint64_t computeHash() const;

    int pieceIndex(Piece piece) const;

    bool isSquareValid(int row, int col) const;

    bool wouldLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol);

    bool hasAnyLegalMove(PieceColor color);

    bool isSquareAttacked(int row, int col, PieceColor color) const;

    bool isPawnMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isRookMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isKnightMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isBishopMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isKingMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    bool isQueenMoveValid(int fromRow, int fromCol, int toRow, int toCol) const;

    void updateKingPosition(PieceColor, int row, int col);
public:
    Board();

    void init();

    uint64_t getHash() const { return currentHash; }

    Piece getPiece(int row, int col) const;

    PieceColor getTurn() const { return turn; }

    bool isGameOver();

    PieceColor getWinner() const { return winner; }

    void makeMove(int fromRow, int fromCol, int toRow, int toCol, PieceType promotionType);

    void undoMove();

    bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol);

    bool isKingInCheck(PieceColor color) const;

    bool isCheckmate(PieceColor color);

    bool isStalemate(PieceColor color);

    void promotePawn(int row, int col, PieceType promotionPiece);
};

