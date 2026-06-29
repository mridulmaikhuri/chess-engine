#pragma once
#include"board.h"
#include<vector>

float evaluate(Board& board);

vector<int> search(Board& board, int depth);