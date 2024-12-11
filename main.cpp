#include <iostream>
#include "welcome_window.h"
#include "config_parser.h"
#include "game_window.h"

int main() {
    Config config = parseConfigFile("../files/config.cfg");

    createGameWindow(config);

    return 0;
}