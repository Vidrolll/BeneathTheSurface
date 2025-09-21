#include <iostream>

#include "Game.h"
#include "util/Input.h"
#include "update/GameStartupUpdateCheck.h"

void Game::update() {
    if (Input::pressed(GLFW_KEY_ESCAPE))
        std::exit(0);
}
void Game::draw() {

}

int main() {
    if (SoftUpdateCheckAndMaybeRun()) {
        std::exit(0);
    }

    Game game;
    game.run();

    return 0;
}