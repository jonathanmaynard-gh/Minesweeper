#include "game_window.h"
#include "leaderboard.h"
#include "welcome_window.h"
#include "minesweeper_logic.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

int calculateRemainingMines(const std::vector<std::vector<bool>> &flagged, int totalMines) {
    int flags = 0;
    for (const auto &row : flagged) {
        for (bool isFlagged : row) {
            if (isFlagged) {
                ++flags;
            }
        }
    }
    return totalMines - flags;
}

bool checkVictory(const std::vector<std::vector<bool>> &revealed, const std::vector<std::vector<int>> &mineGrid) {
    for (size_t r = 0; r < mineGrid.size(); ++r) {
        for (size_t c = 0; c < mineGrid[0].size(); ++c) {
            if (mineGrid[r][c] == 0 && !revealed[r][c]) {
                return false;
            }
        }
    }
    return true;
}

void revealAllMines(std::vector<std::vector<bool>> &revealed, const std::vector<std::vector<int>> &mineGrid) {
    for (size_t r = 0; r < mineGrid.size(); ++r) {
        for (size_t c = 0; c < mineGrid[0].size(); ++c) {
            if (mineGrid[r][c] == 1) {
                revealed[r][c] = true;
            }
        }
    }
}

void autoFlagMines(std::vector<std::vector<bool>> &flagged, const std::vector<std::vector<int>> &mineGrid) {
    for (size_t r = 0; r < mineGrid.size(); ++r) {
        for (size_t c = 0; c < mineGrid[0].size(); ++c) {
            if (mineGrid[r][c] == 1) {
                flagged[r][c] = true;
            }
        }
    }
}

void drawNumber(sf::RenderWindow &window, sf::Texture &digitsTexture, int number, int x, int y, int digits) {
    std::cout << "Drawing number: " << number << " with " << digits << " digits at position (" << x << ", " << y << ")\n";

    if (number < 0) {
        sf::Sprite minusSprite(digitsTexture, sf::IntRect(10 * 21, 0, 21, 32)); // Assuming '-' is the 11th sprite (index 10)
        minusSprite.setPosition(x, y);
        window.draw(minusSprite);
        x += 21;
        number = -number;
        digits--;
    }

    std::string numStr = std::to_string(number);
    while (numStr.length() < digits) {
        numStr = "0" + numStr;
    }

    for (char digit : numStr) {
        int digitIndex = digit - '0';
        sf::Sprite digitSprite(digitsTexture, sf::IntRect(digitIndex * 21, 0, 21, 32)); // Adjust sprite rectangle
        digitSprite.setPosition(x, y);
        window.draw(digitSprite);
        x += 21;
    }
}

void createGameWindow(Config config) {
    std::string playerName;
    if (!showWelcomeWindow(playerName)) {
        std::cerr << "Welcome window closed without entering a name.\n";
        return;
    }

    int tileSize = 32;
    int totalElapsedTime = 0;
    int elapsedTime = 0;

    sf::RenderWindow gameWindow(sf::VideoMode(config.cols * tileSize, config.rows * tileSize + 100), "Minesweeper");

    GameState gameState = GameState::Ongoing;
    auto mineGrid = placeMines(config.rows, config.cols, config.mines);
    auto neighborCounts = calculateNeighborCounts(mineGrid);

    std::vector<std::vector<bool>> revealed(config.rows, std::vector<bool>(config.cols, false));
    std::vector<std::vector<bool>> flagged(config.rows, std::vector<bool>(config.cols, false));

    sf::Font font;
    if (!font.loadFromFile("../files/font.ttf")) {
        std::cerr << "Error: Failed to load font.\n";
        return;
    }

    sf::Texture tileHiddenTexture, tileRevealedTexture, flagTexture, mineTexture;
    sf::Texture faceHappyTexture, faceWinTexture, faceLoseTexture, debugTexture, pauseTexture, playTexture, leaderboardTexture;

    tileHiddenTexture.loadFromFile("../files/images/tile_hidden.png");
    tileRevealedTexture.loadFromFile("../files/images/tile_revealed.png");
    flagTexture.loadFromFile("../files/images/flag.png");
    mineTexture.loadFromFile("../files/images/mine.png");
    faceHappyTexture.loadFromFile("../files/images/face_happy.png");
    faceWinTexture.loadFromFile("../files/images/face_win.png");
    faceLoseTexture.loadFromFile("../files/images/face_lose.png");
    debugTexture.loadFromFile("../files/images/debug.png");
    pauseTexture.loadFromFile("../files/images/pause.png");
    playTexture.loadFromFile("../files/images/play.png");
    leaderboardTexture.loadFromFile("../files/images/leaderboard.png");

    sf::Sprite happyFaceSprite(faceHappyTexture), debugSprite(debugTexture), pauseSprite(pauseTexture), leaderboardSprite(leaderboardTexture);

    int bottomRowY = config.rows * tileSize + 10;

    happyFaceSprite.setPosition((config.cols * 32) / 2 - 16, 32 * (config.rows + 0.5));
    debugSprite.setPosition((config.cols * 32) - 304, 32 * (config.rows + 0.5));
    pauseSprite.setPosition((config.cols * 32) - 240, 32 * (config.rows + 0.5));
    leaderboardSprite.setPosition((config.cols * 32) - 176, 32 * (config.rows + 0.5));

    sf::Clock gameClock;
    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(10, bottomRowY + 40);

    bool debugMode = false;
    bool isPaused = false;

    auto leaderboard = loadLeaderboard("../files/leaderboard.txt");
    bool isLeaderboardOpen = false;

    std::vector<sf::Texture> numberTextures(8);
    for (int i = 0; i < 8; ++i) {
        if (!numberTextures[i].loadFromFile("../files/images/number_" + std::to_string(i + 1) + ".png")) {
            std::cerr << "Error loading number_" << (i + 1) << ".png\n";
        }
    }

    sf::Texture digitsTexture;
    if (!digitsTexture.loadFromFile("../files/images/digits.png")) {
        std::cerr << "Error loading digits.png\n";
    }

    if (!digitsTexture.loadFromFile("../files/images/digits.png")) {
        std::cerr << "Error loading digits.png\n";
    } else {
        std::cout << "Digits texture loaded successfully.\n";
    }


    while (gameWindow.isOpen()) {
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }

            if (!isLeaderboardOpen && gameState == GameState::Ongoing && event.type == sf::Event::MouseButtonPressed && !isPaused) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(gameWindow);
                int col = mousePos.x / tileSize;
                int row = mousePos.y / tileSize;

                if (row >= 0 && row < config.rows && col >= 0 && col < config.cols) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (mineGrid[row][col] == 1) {
                            gameState = GameState::Lost;
                            happyFaceSprite.setTexture(faceLoseTexture);
                            revealAllMines(revealed, mineGrid);
                        } else {
                            revealTile(row, col, mineGrid, revealed, neighborCounts, flagged);
                            if (checkVictory(revealed, mineGrid)) {
                                gameState = GameState::Won;
                                happyFaceSprite.setTexture(faceWinTexture);
                                autoFlagMines(flagged, mineGrid);

                                bool isNewEntry = updateLeaderboard(leaderboard, playerName, elapsedTime);
                                saveLeaderboard("../files/leaderboard.txt", leaderboard);

                                std::cout << "Game won! Player: " << playerName
                                          << " Time: " << elapsedTime
                                          << (isNewEntry ? " (New Entry!)" : "") << "\n";

                                if (isNewEntry) {
                                    showLeaderboardWindow(gameWindow, leaderboard, playerName);
                                } else {
                                    showLeaderboardWindow(gameWindow, leaderboard, "");
                                }
                            }
                        }
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        if (!flagged[row][col]) {
                            flagged[row][col] = true;
                        } else {
                            flagged[row][col] = false;
                        }
                    }
                }
            }

            if (!isLeaderboardOpen && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(gameWindow);

                if (happyFaceSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameState = GameState::Ongoing;
                    happyFaceSprite.setTexture(faceHappyTexture);
                    mineGrid = placeMines(config.rows, config.cols, config.mines);
                    neighborCounts = calculateNeighborCounts(mineGrid);
                    revealed.assign(config.rows, std::vector<bool>(config.cols, false));
                    flagged.assign(config.rows, std::vector<bool>(config.cols, false));
                    totalElapsedTime = 0;
                    pauseSprite.setTexture(pauseTexture);
                    elapsedTime = 0;
                    gameClock.restart();
                    isPaused = false;
                } else if (debugSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (gameState == GameState::Ongoing) {
                        debugMode = !debugMode;
                    }
                } else if (pauseSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (gameState == GameState::Ongoing) {
                        isPaused = !isPaused;

                        if (isPaused) {
                            pauseSprite.setTexture(playTexture);
                            totalElapsedTime += static_cast<int>(gameClock.getElapsedTime().asSeconds());
                        } else {
                            pauseSprite.setTexture(pauseTexture);
                            gameClock.restart();
                        }
                    }

                } else if (leaderboardSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    totalElapsedTime += static_cast<int>(gameClock.getElapsedTime().asSeconds());
                    isPaused = true;

                    auto tempRevealed = revealed;

                    for (size_t r = 0; r < revealed.size(); ++r) {
                        for (size_t c = 0; c < revealed[0].size(); ++c) {
                            revealed[r][c] = true;
                        }
                    }

                    gameWindow.clear(sf::Color::White);
                    for (int r = 0; r < config.rows; ++r) {
                        for (int c = 0; c < config.cols; ++c) {
                            sf::Sprite tileSprite(tileHiddenTexture);
                            tileSprite.setPosition(c * tileSize, r * tileSize);

                            if (revealed[r][c]) {
                                tileSprite.setTexture(tileRevealedTexture);
                            } else if (flagged[r][c]) {
                                tileSprite.setTexture(flagTexture);
                            } else {
                                tileSprite.setTexture(tileHiddenTexture);
                            }

                            gameWindow.draw(tileSprite);
                        }
                    }
                    gameWindow.draw(happyFaceSprite);
                    gameWindow.draw(debugSprite);
                    gameWindow.draw(pauseSprite);
                    gameWindow.draw(leaderboardSprite);
                    int minutes = elapsedTime / 60;
                    int seconds = elapsedTime % 60;
                    int remainingFlags = calculateRemainingMines(flagged, config.mines);
                    drawNumber(gameWindow, digitsTexture, remainingFlags, 33, (32 * (config.rows + 0.5)) + 16, 3);
                    drawNumber(gameWindow, digitsTexture, minutes, (config.cols * 32) - 97, (32 * (config.rows + 0.5)) + 16, 2);
                    drawNumber(gameWindow, digitsTexture, seconds, (config.cols * 32) - 54, (32 * (config.rows + 0.5)) + 16, 2);
                    gameWindow.display();

                    isLeaderboardOpen = true;
                    showLeaderboardWindow(gameWindow, leaderboard, "Player");

                    revealed = tempRevealed;
                    isLeaderboardOpen = false;

                    isPaused = false;
                    gameClock.restart();
                }
            }
        }

        sf::Sprite testDigitSprite(digitsTexture, sf::IntRect(0 * 21, 0, 21, 32));
        testDigitSprite.setPosition(100, 100);
        gameWindow.draw(testDigitSprite);

        if (gameState == GameState::Ongoing) {
            if (isPaused) {
                elapsedTime = totalElapsedTime;
            } else {
                elapsedTime = totalElapsedTime + static_cast<int>(gameClock.getElapsedTime().asSeconds());
            }
        }

        int minutes = elapsedTime / 60;
        int seconds = elapsedTime % 60;

        gameWindow.clear(sf::Color::White);

        for (int r = 0; r < config.rows; ++r) {
            for (int c = 0; c < config.cols; ++c) {
                sf::Sprite tileSprite(tileHiddenTexture);
                tileSprite.setPosition(c * tileSize, r * tileSize);

                if (isLeaderboardOpen) {
                    tileSprite.setTexture(tileRevealedTexture);
                } else if (isPaused) {
                    tileSprite.setTexture(tileRevealedTexture);
                } else if (revealed[r][c]) {
                    if (mineGrid[r][c] == 1) {
                        tileSprite.setTexture(mineTexture);
                    } else if (neighborCounts[r][c] > 0) {
                        sf::Sprite numberSprite(numberTextures[neighborCounts[r][c] - 1]);
                        numberSprite.setPosition(c * tileSize, r * tileSize);
                        gameWindow.draw(numberSprite);
                        continue;
                    } else {
                        tileSprite.setTexture(tileRevealedTexture);
                    }
                } else if (flagged[r][c]) {
                    tileSprite.setTexture(flagTexture);
                } else if (debugMode && mineGrid[r][c] == 1) {
                    tileSprite.setTexture(mineTexture);
                    tileSprite.setColor(sf::Color(255, 255, 255, 128));
                }

                gameWindow.draw(tileSprite);
            }
        }


        gameWindow.draw(happyFaceSprite);
        gameWindow.draw(debugSprite);
        gameWindow.draw(pauseSprite);
        gameWindow.draw(leaderboardSprite);
        gameWindow.draw(timerText);

        int remainingFlags = calculateRemainingMines(flagged, config.mines);
        drawNumber(gameWindow, digitsTexture, remainingFlags, 33, (32 * (config.rows + 0.5)) + 16, 3);
        drawNumber(gameWindow, digitsTexture, minutes, (config.cols * 32) - 97, (32 * (config.rows + 0.5)) + 16, 2);
        drawNumber(gameWindow, digitsTexture, seconds, (config.cols * 32) - 54, (32 * (config.rows + 0.5)) + 16, 2);

        gameWindow.display();

        std::cout << "Elapsed time: " << elapsedTime << "\n";
        std::cout << "Remaining flags: " << remainingFlags << "\n";
    }
}