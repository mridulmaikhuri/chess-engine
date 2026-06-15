#include "board.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
using namespace std;

Board::Board() {
    init();
}   

void Board::init() {
    historySize = 0;

    turn = WHITE;

    whiteKingMoved = false;
    blackKingMoved = false;

    whiteKingsideRookMoved = false;
    whiteQueensideRookMoved = false;
    blackKingsideRookMoved = false;
    blackQueensideRookeMoved = false;

    enPassantAvailable = false;
    enPassantRow = -1;
    enPassantCol = -1;

    gameOver = false;
    winner = NONE;

    whiteKingRow = 7;
    whiteKingCol = 4;
    blackKingRow = 0;
    blackKingCol = 4;
     
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

void Board::updateKingPosition(PieceColor color, int row, int col) {
    if (color == WHITE) {
        whiteKingRow = row;
        whiteKingCol = col;
    } else {
        blackKingRow = row;
        blackKingCol = col;
    }
}

void Board::movePiece(int fromRow, int fromCol, int toRow, int toCol, PieceType promotionType) {

    Piece piece = board[fromRow][fromCol];
    Piece captured = board[toRow][toCol];

    // Build history
    MoveRecord rec;

    rec.fromRow = fromRow;
    rec.fromCol = fromCol;
    rec.toRow = toRow;
    rec.toCol = toCol;
    rec.movedPiece = piece;
    rec.capturedPiece = captured;
    rec.promotedTo = promotionType;

    rec.prevEnPassantAvailable = enPassantAvailable;
    rec.prevEnPassantRow = enPassantRow;
    rec.prevEnPassantCol = enPassantCol;

    rec.prevWhiteKingMoved = whiteKingMoved;
    rec.prevBlackKingMoved = blackKingMoved;
    rec.prevWhiteKingSideRookMoved = whiteKingsideRookMoved;
    rec.prevWhiteQueenSideRookMoved = whiteQueensideRookMoved;
    rec.prevBlackKingSideRookMoved = blackKingsideRookMoved;
    rec.prevBlackQueenSideRookMoved = blackQueensideRookeMoved;

    rec.wasEnPassant = false;
    rec.wasCastle = false;

    // en-passant
    if (piece.type == PAWN && abs(toCol - fromCol) == 1 && board[toRow][toCol].type == EMPTY
        && enPassantAvailable && toRow == enPassantRow && toCol == enPassantCol) 
    {
        int capturedRow = (piece.color == WHITE) ? toRow + 1 : toRow - 1;
        rec.wasEnPassant = true;
        rec.enPassantCapturedRow = capturedRow;
        rec.enPassantCapturedCol = toCol;
        rec.capturedPiece = board[capturedRow][toCol];
        board[capturedRow][toCol] = {EMPTY, NONE};
    }

    // castling
    if (piece.type == KING && abs(toCol - fromCol) == 2) {
        rec.wasCastle = true;

        if (toCol > fromCol) {
            rec.rookFromCol = 7;
            rec.rookToCol = toCol - 1;
        } else {
            rec.rookFromCol = 0;
            rec.rookToCol = toCol + 1;
        }

        board[fromRow][rec.rookToCol] = board[fromRow][rec.rookFromCol];
        board[fromRow][rec.rookFromCol] = {EMPTY, NONE};
    }

    // execute the move
    board[toRow][toCol] = piece;
    board[fromRow][fromCol] = {EMPTY, NONE};

    // update king position and castling rights
    if (piece.type == KING) {
        updateKingPosition(piece.color, toRow, toCol);
        (piece.color == WHITE) ? whiteKingMoved = true : blackKingMoved = true;
    }

    if (piece.type == ROOK) {
        if (fromRow == 7 && fromCol == 0) whiteQueensideRookMoved = true;
        if (fromRow == 7 && fromCol == 7) whiteKingsideRookMoved = true;
        if (fromRow == 0 && fromCol == 0) blackQueensideRookeMoved = true;
        if (fromRow == 0 && fromCol == 7) blackKingsideRookMoved = true;
    }

    if (captured.type == ROOK) {
        if (toRow == 7 && toCol == 0) whiteQueensideRookMoved = true;
        if (toRow == 7 && toCol == 7) whiteKingsideRookMoved = true;
        if (toRow == 0 && toCol == 0) blackQueensideRookeMoved = true;
        if (toRow == 0 && toCol == 7) blackKingsideRookMoved = true;
    }

    // enPassant flag for next move
    if (piece.type == PAWN && abs(toRow - fromRow) == 2) {
        enPassantAvailable = true;
        enPassantRow = (fromRow + toRow) / 2;;
        enPassantCol = fromCol;
    } else {
        enPassantAvailable = false;
        enPassantRow = -1;
        enPassantCol = -1;
    }

    // Pawn promotion
    if (piece.type == PAWN && ((piece.color == WHITE && toRow == 0) || (piece.color == BLACK && toRow == 7))) { 
        promotePawn(toRow, toCol, promotionType );
    }

    // Save Record
    if (historySize < 1024) history[historySize++] = rec;

    // switch turn
    turn = (turn == WHITE) ? BLACK : WHITE;
}

void Board::undoMove() {
    if (historySize == 0) return;

    MoveRecord rec = history[--historySize];

    board[rec.fromRow][rec.fromCol] = rec.movedPiece;
    board[rec.toRow][rec.toCol] = rec.capturedPiece;

    if (rec.promotedTo != EMPTY) {
        board[rec.fromRow][rec.fromCol].type = PAWN;
    }

    if (rec.wasCastle) {
        board[rec.fromRow][rec.rookFromCol] = board[rec.fromRow][rec.rookToCol];
        board[rec.fromRow][rec.rookToCol] = {EMPTY, NONE};
    }

    if (rec.wasEnPassant) {
        board[rec.enPassantCapturedRow][rec.enPassantCapturedCol] = rec.capturedPiece;
        board[rec.toRow][rec.toCol] = {EMPTY, NONE};
    }

    if (rec.movedPiece.type == KING) {
        updateKingPosition(rec.movedPiece.color, rec.fromRow, rec.fromCol);
    }

    enPassantAvailable = rec.prevEnPassantAvailable;
    enPassantRow = rec.prevEnPassantRow;
    enPassantCol = rec.prevEnPassantCol;

    whiteKingMoved = rec.prevWhiteKingMoved;
    blackKingMoved = rec.prevBlackKingMoved;
    whiteKingsideRookMoved = rec.prevWhiteKingSideRookMoved;
    whiteQueensideRookMoved = rec.prevWhiteQueenSideRookMoved;
    blackKingsideRookMoved = rec.prevBlackKingSideRookMoved;
    blackQueensideRookeMoved = rec.prevBlackQueenSideRookMoved;

    gameOver = false;
    winner = NONE;

    turn = (turn == WHITE) ? BLACK : WHITE;
}

bool Board::isMoveLegal(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isSquareValid(fromRow, fromCol) || !isSquareValid(toRow, toCol)) return false;
    if (fromRow == toRow && fromCol == toCol) return false;

    Piece fromPiece = board[fromRow][fromCol];
    Piece toPiece = board[toRow][toCol];

    if (fromPiece.type == EMPTY) return false;
    if (fromPiece.color == toPiece.color) return false;

    switch (fromPiece.type) {
        case PAWN:
            return isPawnMoveValid(fromRow, fromCol, toRow, toCol);
        
        case ROOK:
            return isRookMoveValid(fromRow, fromCol, toRow, toCol);
        
        case KNIGHT:
            return isKnightMoveValid(fromRow, fromCol, toRow, toCol);

        case BISHOP:
            return isBishopMoveValid(fromRow, fromCol, toRow, toCol);
        
        case QUEEN:
            return isQueenMoveValid(fromRow, fromCol, toRow, toCol);
        
        case KING:
            return isKingMoveValid(fromRow, fromCol, toRow, toCol);
        
        default:
            return false;
    }
}   

bool Board::isMoveValid(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isSquareValid(fromRow, fromCol) || !isSquareValid(toRow, toCol)) return false;

    Piece piece = board[fromRow][fromCol];

    if (piece.color != turn) return false;

    if (!isMoveLegal(fromRow, fromCol, toRow, toCol)) return false;

    if (wouldLeaveKingInCheck(fromRow, fromCol, toRow, toCol)) return false;

    return true;
}

bool Board::isPawnMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
    Piece piece = board[fromRow][fromCol];
    Piece target = board[toRow][toCol];
    
    int dir = (piece.color == WHITE) ? -1 : 1;
    int stRow = (piece.color == WHITE) ? 6 : 1;

    if (fromCol == toCol && toRow == fromRow + dir && target.type == EMPTY) {
        return true;
    }

    if (fromCol == toCol && fromRow == stRow && toRow == fromRow + 2 * dir 
        && target.type == EMPTY && board[fromRow + dir][fromCol].type == EMPTY) 
    {  
        return true;
    }

    if (abs(toCol - fromCol) == 1 && toRow == fromRow + dir 
        && target.type != EMPTY && target.color != piece.color) 
    {
        return true;
    }

    if (abs(toCol - fromCol) == 1 && toRow == fromRow + dir
        && target.type == EMPTY && enPassantAvailable && toRow == enPassantRow
        && toCol == enPassantCol) 
    {
        return true;
    }

    return false;
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

    Piece king = board[fromRow][fromCol];

    if (max(rowDiff, colDiff) == 1) return true;

    // castling
    if (rowDiff != 0 || colDiff != 2) return false;

    bool kingMoved = (king.color == WHITE) ? whiteKingMoved : blackKingMoved;
    int backRank = (king.color == WHITE) ? 7 : 0;

    if (kingMoved || fromRow != backRank || fromCol != 4) return false;

    if (isKingInCheck(king.color)) return false;
    
    PieceColor enemy = (king.color == WHITE) ? BLACK : WHITE;

    if (toCol == 6) {
        bool rookMoved = (king.color == WHITE) ? whiteKingsideRookMoved : blackKingsideRookMoved;
        if (rookMoved) return false;
        if (board[backRank][5].type != EMPTY || board[backRank][6].type != EMPTY) return false;
        if (isSquareAttacked(backRank, 5, enemy)) return false;
        if (isSquareAttacked(backRank, 6, enemy)) return false;
    } else if (toCol == 2) {
        bool rookMoved = (king.color == WHITE) ? whiteQueensideRookMoved : blackQueensideRookeMoved;
        if (rookMoved) return false;
        if (board[backRank][1].type != EMPTY || board[backRank][2].type != EMPTY
            || board[backRank][3].type != EMPTY) return false;
        if (isSquareAttacked(backRank, 3, enemy)) return false;
        if (isSquareAttacked(backRank, 2, enemy)) return false; 
    } else {
        return false;
    }

    return true;
}

bool Board::isKingInCheck(PieceColor color) const {
    int kingRow = (color == WHITE) ? whiteKingRow : blackKingRow;
    int kingCol = (color == WHITE) ? whiteKingCol : blackKingCol;

    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;
    return isSquareAttacked(kingRow, kingCol, enemy);   
}

bool Board::wouldLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol) {
    movePiece(fromRow, fromCol, toRow, toCol, EMPTY);
    bool res = isKingInCheck((turn == WHITE) ? BLACK : WHITE);
    undoMove();
    return res;
}

bool Board::hasAnyLegalMove(PieceColor color) {
    PieceColor originalTurn = turn;
    turn = color;

    for (int fromRow = 0;fromRow < BOARD_SIZE;fromRow ++) {
        for (int fromCol = 0;fromCol < BOARD_SIZE;fromCol ++) {
            Piece piece = board[fromRow][fromCol];

            if (piece.color != color) continue;
            
            for (int toRow = 0;toRow < BOARD_SIZE;toRow ++) {
                for (int toCol = 0;toCol < BOARD_SIZE;toCol ++) {
                    if (isMoveValid(fromRow, fromCol, toRow, toCol)) {
                        turn = originalTurn;
                        return true;
                    }
                }
            }
        }
    }

    turn = originalTurn;
    return false;
}

bool Board::isCheckmate(PieceColor color) {
    return isKingInCheck(color) && !hasAnyLegalMove(color);
}

bool Board::isStalemate(PieceColor color) {
    return !isKingInCheck(color) && !hasAnyLegalMove(color);
}

bool Board::isGameOver() {
    // check for checkmate
    if (isCheckmate(turn)) {
        gameOver = true;
        winner = (turn == WHITE) ? BLACK : WHITE;
        return true;
    }

    // check for stalemate
    if (isStalemate(turn)) {
        gameOver = true;
        winner = NONE;
        return true;
    }

    return false;
}

void Board::promotePawn(int row, int col, PieceType promotionPiece) {
    Piece &piece = board[row][col];

    if (piece.type != PAWN) return;

    if ((piece.color == WHITE && row == 0) ||
        (piece.color == BLACK && row == 7)) 
    {
        piece.type = promotionPiece;
    }
}

bool Board::isSquareAttacked(int row, int col, PieceColor color) const {
    for (int r = 0;r < BOARD_SIZE;r ++) {
        for (int c = 0;c < BOARD_SIZE;c ++) {
            Piece piece = board[r][c];

            if (piece.color != color) continue;

            switch (piece.type) {
                case PAWN: {
                    int dir = (color == WHITE) ? -1 : 1;
                    if (r + dir == row && abs(col - c) == 1) return true;
                    break;
                }
                
                case ROOK:
                    if (isRookMoveValid(r, c, row, col)) return true;
                    break;
                
                case KNIGHT:
                    if (isKnightMoveValid(r, c, row, col)) return true;
                    break;

                case BISHOP:
                    if (isBishopMoveValid(r, c, row, col)) return true;
                    break;
                
                case QUEEN:
                    if (isQueenMoveValid(r, c, row, col)) return true;
                    break;
                
                case KING:
                    if (abs(r - row) <= 1 && abs(c - col) <= 1)
                        return true;
                    break;

                default:
                    break;
            }
        }
    }

    return false;
}
