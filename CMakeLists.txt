cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Project3)

set(SFML_DIR "C:/Users/jonat/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)

# Include all required source files
add_executable(Project3
        main.cpp
        welcome_window.cpp
        config_parser.cpp
        game_window.cpp
        minesweeper_logic.cpp
        leaderboard.cpp
)

# Link SFML libraries
target_link_libraries(Project3 sfml-system sfml-window sfml-graphics sfml-audio)
