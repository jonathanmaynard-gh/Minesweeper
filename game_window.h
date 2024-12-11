#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include "config_parser.h"
#include <SFML/Graphics.hpp>
#include <vector>

enum class GameState {
    Ongoing,
    Won,
    Lost
};

void createGameWindow(Config config);

int calculateRemainingMines(const std::vector<std::vector<bool>> &flagged, int totalMines);

void drawNumber(sf::RenderWindow &window, sf::Texture &digitsTexture, int number, int x, int y);

#endif
