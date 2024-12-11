#include "leaderboard.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

int parseTime(const std::string &timeStr) {
    size_t colonPos = timeStr.find(':');
    if (colonPos == std::string::npos) {
        throw std::invalid_argument("Invalid time format");
    }

    int minutes = std::stoi(timeStr.substr(0, colonPos));
    int seconds = std::stoi(timeStr.substr(colonPos + 1));
    return minutes * 60 + seconds;
}

std::string formatTime(int totalSeconds) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    std::ostringstream timeStream;
    timeStream << std::setw(2) << std::setfill('0') << minutes << ":"
               << std::setw(2) << std::setfill('0') << seconds;
    return timeStream.str();
}

std::vector<LeaderboardEntry> loadLeaderboard(const std::string &filePath) {
    std::vector<LeaderboardEntry> leaderboard;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open leaderboard file.\n";
        return leaderboard;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t commaPos = line.find(',');
        if (commaPos == std::string::npos) continue;

        try {
            std::string timeStr = line.substr(0, commaPos);
            std::string name = line.substr(commaPos + 1);
            int totalSeconds = parseTime(timeStr);

            if (!name.empty()) {
                leaderboard.push_back({name, totalSeconds});
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: Invalid leaderboard entry: " << line << " (" << e.what() << ")\n";
        }
    }

    file.close();
    return leaderboard;
}

void saveLeaderboard(const std::string &filename, const std::vector<LeaderboardEntry> &leaderboard) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to save leaderboard file.\n";
        return;
    }

    for (const auto &entry : leaderboard) {
        file << formatTime(entry.time) << "," << entry.playerName << "\n";
    }

    std::cout << "Leaderboard saved to file: " << filename << "\n";
    file.close();
}

bool updateLeaderboard(std::vector<LeaderboardEntry> &leaderboard, const std::string &playerName, int time) {
    LeaderboardEntry newEntry = {playerName, time};
    leaderboard.push_back(newEntry);

    std::sort(leaderboard.begin(), leaderboard.end(), [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
        return a.time < b.time;
    });

    if (leaderboard.size() > 5) {
        leaderboard.pop_back();
    }

    for (const auto &entry : leaderboard) {
        if (entry.playerName == playerName && entry.time == time) {
            return true;
        }
    }
    return false;
}

void showLeaderboardWindow(sf::RenderWindow &gameWindow, const std::vector<LeaderboardEntry> &leaderboard, const std::string &highlightPlayer) {
    sf::Font font;
    if (!font.loadFromFile("../files/font.ttf")) {
        std::cerr << "Error: Failed to load font for leaderboard.\n";
        return;
    }

    sf::RenderWindow leaderboardWindow(sf::VideoMode(400, 300), "Leaderboard");

    // Title
    sf::Text title("LEADERBOARD", font, 20);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setFillColor(sf::Color::White);
    title.setPosition(400 / 2.0f - title.getLocalBounds().width / 2, 20);

    while (leaderboardWindow.isOpen()) {
        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leaderboardWindow.close();
            }
        }

        leaderboardWindow.clear(sf::Color::Blue);
        leaderboardWindow.draw(title);

        float rankX = 65;
        float timeX = 160;
        float nameX = 300;
        float startY = 60;
        float rowSpacing = 40;

        for (size_t i = 0; i < leaderboard.size(); ++i) {
            const auto &entry = leaderboard[i];

            sf::Text rank(std::to_string(i + 1) + ".", font, 18);
            rank.setFillColor(sf::Color::White);
            rank.setPosition(rankX, startY + i * rowSpacing);
            leaderboardWindow.draw(rank);

            std::string formattedTime = formatTime(entry.time);
            sf::Text timeText(formattedTime, font, 18);
            timeText.setFillColor(sf::Color::White);
            timeText.setPosition(timeX, startY + i * rowSpacing);
            leaderboardWindow.draw(timeText);

            sf::Text nameText(entry.playerName, font, 18);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(nameX, startY + i * rowSpacing);
            if (entry.playerName == highlightPlayer) {
                nameText.setString(entry.playerName + " *");
            }
            leaderboardWindow.draw(nameText);
        }


        leaderboardWindow.display();
    }
}