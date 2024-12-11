#include "minesweeper_logic.h"
#include <set>
#include <cstdlib>
#include <ctime>

std::vector<std::vector<int>> placeMines(int rows, int cols, int mines) {
    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));
    std::set<std::pair<int, int>> minePositions;

    srand(static_cast<unsigned>(time(nullptr)));

    while (minePositions.size() < mines) {
        int r = rand() % rows;
        int c = rand() % cols;

        if (minePositions.find({r, c}) == minePositions.end()) {
            minePositions.insert({r, c});
            grid[r][c] = 1;
        }
    }

    return grid;
}

std::vector<std::vector<int>> calculateNeighborCounts(const std::vector<std::vector<int>> &mineGrid) {
    int rows = mineGrid.size();
    int cols = mineGrid[0].size();
    std::vector<std::vector<int>> neighborCounts(rows, std::vector<int>(cols, 0));

    const int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1},
                                  {0, -1},          {0, 1},
                                  {1, -1}, {1, 0}, {1, 1}};

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (mineGrid[r][c] == 1) {
                continue;
            }

            int mineCount = 0;
            for (const auto &dir : directions) {
                int nr = r + dir[0];
                int nc = c + dir[1];

                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && mineGrid[nr][nc] == 1) {
                    ++mineCount;
                }
            }
            neighborCounts[r][c] = mineCount;
        }
    }

    return neighborCounts;
}

void revealTile(int r, int c, const std::vector<std::vector<int>> &mineGrid,
                std::vector<std::vector<bool>> &revealed,
                const std::vector<std::vector<int>> &neighborCounts,
                const std::vector<std::vector<bool>> &flagged) {
    if (r < 0 || r >= mineGrid.size() || c < 0 || c >= mineGrid[0].size() || revealed[r][c] || flagged[r][c]) {
        return;
    }

    revealed[r][c] = true;

    if (neighborCounts[r][c] == 0 && mineGrid[r][c] == 0) {
        const int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1},
                                      {0, -1},          {0, 1},
                                      {1, -1}, {1, 0}, {1, 1}};
        for (const auto &dir : directions) {
            revealTile(r + dir[0], c + dir[1], mineGrid, revealed, neighborCounts, flagged);
        }
    }
}
