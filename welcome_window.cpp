#include "welcome_window.h"
#include <SFML/Graphics.hpp>
#include <cctype>

void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

bool showWelcomeWindow(std::string &playerName) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Welcome to Minesweeper", sf::Style::Close);
    sf::Font font;
    if (!font.loadFromFile("../files/font.ttf")) {
        return false;
    }

    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    welcomeText.setFillColor(sf::Color::White);
    setText(welcomeText, 400, 100);

    sf::Text promptText("Enter your name:", font, 20);
    promptText.setStyle(sf::Text::Bold);
    promptText.setFillColor(sf::Color::White);
    setText(promptText, 400, 200);

    sf::Text nameText("", font, 18);
    nameText.setFillColor(sf::Color::Yellow);
    setText(nameText, 400, 225);

    std::string name;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return false;
            }
            else if (event.type == sf::Event::TextEntered) {
                char input = static_cast<char>(event.text.unicode);
                if (std::isalpha(input) && name.size() < 10) {
                    if (name.empty()) input = std::toupper(input);
                    else input = std::tolower(input);
                    name += input;
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Backspace && !name.empty()) {
                    name.pop_back();
                }
                else if (event.key.code == sf::Keyboard::Enter && !name.empty()) {
                    playerName = name;
                    window.close();
                }
            }
        }

        nameText.setString(name + "|");
        window.clear(sf::Color::Blue);
        window.draw(welcomeText);
        window.draw(promptText);
        window.draw(nameText);
        window.display();
    }

    return true;
}