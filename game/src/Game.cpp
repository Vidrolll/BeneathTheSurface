//
// Created by Caden Vize on 9/21/2025.
//

#include <iostream>

#include "Game.h"
#include "util/Input.h"
#include "update/GameStartupUpdateCheck.h"

#include "graphics/Texture.h"
#include "util/Scene.h"

GLint texture;

Scene scene;

void Game::update() {
    if (Input::pressed(GLFW_KEY_ESCAPE))
        std::exit(0);
    float speed = (Input::down(GLFW_KEY_LEFT_SHIFT) ? 1.f : 10.f);
    if (Input::down(GLFW_KEY_LEFT))
        gamecam::cam.move({-speed,0,0});
    if (Input::down(GLFW_KEY_RIGHT))
        gamecam::cam.move({speed,0,0});
    if (Input::down(GLFW_KEY_UP))
        gamecam::cam.move({0,speed,0});
    if (Input::down(GLFW_KEY_DOWN))
        gamecam::cam.move({0,-speed,0});
    scene.update();
}

void drawTile(int x, int y) {
    int tileSize = 100;
    glColor3f(1.f, 1.f, 1.f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_POLYGON);
    glTexCoord2i(0,0); glVertex3f(x*tileSize, y*tileSize, 0.f);
    glTexCoord2i(1,0); glVertex3f(x*tileSize+tileSize, y*tileSize, 0.f);
    glTexCoord2i(1,1); glVertex3f(x*tileSize+tileSize, y*tileSize+tileSize, 0.f);
    glTexCoord2i(0,1); glVertex3f(x*tileSize, y*tileSize+tileSize, 0.f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glColor3f(.0f, 1.0f, .0f);
    glBegin(GL_POLYGON);
    glVertex3f(x*tileSize+tileSize, y*tileSize+tileSize, 0.f);
    glVertex3f(x*tileSize+tileSize, y*tileSize+tileSize, -tileSize);
    glVertex3f(x*tileSize, y*tileSize+tileSize, -tileSize);
    glVertex3f(x*tileSize, y*tileSize+tileSize, 0.f);
    glEnd();
    glColor3f(1.0f, .0f, .0f);
    glBegin(GL_POLYGON);
    glVertex3f(x*tileSize+tileSize, y*tileSize, 0.f);
    glVertex3f(x*tileSize+tileSize, y*tileSize, -tileSize);
    glVertex3f(x*tileSize+tileSize, y*tileSize+tileSize, -tileSize);
    glVertex3f(x*tileSize+tileSize, y*tileSize+tileSize, 0.f);
    glEnd();
}

void Game::draw() {
    scene.draw();
    //
    // drawTile(1,1);
    // drawTile(1,2);
    // drawTile(2,1);
    // drawTile(3,1);
    // drawTile(4,1);
    //
    // glColor3f(0.f,0.f,1.f);
    // glBegin(GL_POLYGON);
    // glVertex3f(3*50, 2*50, -25.f);
    // glVertex3f(3*50+50, 2*50, -25.f);
    // glVertex3f(3*50+50, 2*50+100, -25.f);
    // glVertex3f(3*50, 2*50+100, -25.f);
    // glEnd();
    //
    // glColor3f(1.f,0.f,1.f);
    // glBegin(GL_POLYGON);
    // glVertex3f(50, 2*50, -50.f);
    // glVertex3f(250, 2*50, -50.f);
    // glVertex3f(250, 2*50+100, -50.f);
    // glVertex3f(50, 2*50+100, -50.f);
    // glEnd();
    //
    // glColor3f(1.0f, 0.f, 0.f);
    // glBegin(GL_POLYGON);
    // glVertex3f(-25.0f, -25.0f, -500.f);
    // glVertex3f(25.0f, -25.0f, -500.f);
    // glVertex3f(25.0f, 25.0f, -500.f);
    // glVertex3f(-25.0f, 25.0f, -500.f);
    // glEnd();

    // for (int x = 0; x < 100; x++) {
    //     for (int y = 0; y < 100; y++) {
    //         int tileSize = 10;
    //         float color = (x+y) % 2 == 0 ? 1.0f : 0.0f;
    //         glColor3f(color, color, color);
    //         glBegin(GL_POLYGON);
    //         glVertex3f(x*tileSize, y*tileSize, -50.f);
    //         glVertex3f(x*tileSize+tileSize, y*tileSize, -50.f);
    //         glVertex3f(x*tileSize+tileSize, y*tileSize+tileSize, -50.f);
    //         glVertex3f(x*tileSize, y*tileSize+tileSize, -50.f);
    //         glEnd();
    //     }
    // }
    drawTile(5,5);
}

int main() {
#ifdef ENABLE_AUTO_UPDATE
    if (SoftUpdateCheckAndMaybeRun()) {
        std::exit(0);
    }
#endif

    Game game;
    texture = LoadTexture2D("resources/textures/block_sprite.png");
    scene = Scene();
    game.run();

    return 0;
}