#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

struct LeaderboardEntry {
    std::string playerName;
    int time;
};

std::vector<LeaderboardEntry> loadLeaderboard(const std::string &filename);
void saveLeaderboard(const std::string &filename, const std::vector<LeaderboardEntry> &leaderboard);
bool updateLeaderboard(std::vector<LeaderboardEntry> &leaderboard, const std::string &playerName, int time);
void showLeaderboardWindow(sf::RenderWindow &gameWindow, const std::vector<LeaderboardEntry> &leaderboard, const std::string &highlightPlayer = "");

#endif
