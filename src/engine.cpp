#include"board.h"
#include<vector>
#include<iostream>
using namespace std;

// TODO: add logic for checkmate in evaluate function.

float evaluate(Board& board) {
    
    float score = 0;
    for (int row = 0;row < 8;row ++) {
        for (int col = 0;col < 8;col ++) {
            float sc = 0;
            switch (board.getPiece(row, col).type)
            {
                case PAWN:
                    sc = 1;
                    break;
                case ROOK:
                    sc = 5;
                    break;
                case KNIGHT:
                    sc = 3;
                    break;
                case BISHOP:
                    sc = 3.1;
                    break;
                case QUEEN:
                    sc = 9;
                    break;
                case KING:
                    sc = 1000;
                    break;
                default:
                    break;
            }
            if (board.getPiece(row, col).color == BLACK) sc *= -1;
            score += sc;
        }
    }
    return score;
}

vector<int> search(Board& board, int depth) {
    // minimise the eval score while playing for black while maximise the eval score while playing for white
    auto search_helper = [&](auto&& self, int depth) -> float {
        if (depth == 0) {
            return evaluate(board);
        }
        float score = 2e9;
        if (board.getTurn() == WHITE) score *= -1;
        //cout << score << "\n";
        for (int fr = 0;fr < 8;fr ++) {
            for (int fc = 0;fc < 8;fc ++) {
                if (board.getPiece(fr, fc).color == board.getTurn()) {
                    for (int tr = 0;tr < 8;tr ++) {
                        for (int tc = 0;tc < 8;tc ++) {
                            if (board.isMoveValid(fr, fc, tr, tc)) {
                                PieceColor player = board.getTurn();
                                board.makeMove(fr, fc, tr, tc, QUEEN);
                                float sc = 0;
                                if (board.isGameOver()) {
                                    if (board.getWinner() == WHITE) sc = 1e9;
                                    else if (board.getWinner() == BLACK) sc = -1e9;
                                } else {
                                    sc = self(self, depth - 1);
                                }
                                //cout << sc << "\n";
                                if (player == WHITE) score = max(score, sc);
                                else score = min(score, sc);
                                board.undoMove();
                            }
                        }
                    }
                }
            }
        }
        //cout << score << "\n";
        return score;
    };

    float best_sc = 2e9;
    int frow = -1, fcol = -1;
    int trow = -1, tcol = -1;
    for (int fr = 0;fr < 8;fr ++) {
        for (int fc = 0;fc < 8;fc ++) {
            if (board.getPiece(fr, fc).color == board.getTurn()) {
                for (int tr = 0;tr < 8;tr ++) {
                    for (int tc = 0;tc < 8;tc ++) {
                        if (board.isMoveValid(fr, fc, tr, tc)) {
                            board.makeMove(fr, fc, tr, tc, QUEEN);
                            float score = 0;
                            if (board.isGameOver()) {
                                if (board.getWinner() == WHITE) score = 1e9;
                                else if (board.getWinner() == BLACK) score = -1e9;
                            } else {
                                score = search_helper(search_helper, depth - 1);
                            }
                            if (score < best_sc) {
                                best_sc = score;
                                frow = fr;
                                fcol = fc;
                                trow = tr;
                                tcol = tc;
                            }
                            board.undoMove();
                        }
                    }
                }
            }
        }
    }
    return {frow, fcol, trow, tcol};
}