//
// Created by Caden Vize on 9/21/2025.
//

#include <iostream>

#include "Game.h"
#include "util/Input.h"
#include "update/GameStartupUpdateCheck.h"

void Game::update() {
    if (Input::pressed(GLFW_KEY_ESCAPE))
        std::exit(0);
}
void Game::draw() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(50.0f, 50.0f);
    glVertex2f(100.0f, 50.0f);
    glVertex2f(100.0f, 100.0f);
    glVertex2f(50.0f, 100.0f);
    glEnd();
}

int main() {
#ifdef ENABLE_AUTO_UPDATE
    if (SoftUpdateCheckAndMaybeRun()) {
        std::exit(0);
    }
#endif

    Game game;
    game.run();

    return 0;
}