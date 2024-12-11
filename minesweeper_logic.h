#ifndef MINESWEEPER_LOGIC_H
#define MINESWEEPER_LOGIC_H

#include <vector>

std::vector<std::vector<int>> placeMines(int rows, int cols, int mines);

std::vector<std::vector<int>> calculateNeighborCounts(const std::vector<std::vector<int>> &mineGrid);

void revealTile(int r, int c, const std::vector<std::vector<int>> &mineGrid,
                std::vector<std::vector<bool>> &revealed,
                const std::vector<std::vector<int>> &neighborCounts,
                const std::vector<std::vector<bool>> &flagged);


#endif