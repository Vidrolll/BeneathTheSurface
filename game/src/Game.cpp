//
// Created by Caden Vize on 9/21/2025.
//

#include <iostream>

#include "Game.h"
#include "util/Input.h"
#include "update/GameStartupUpdateCheck.h"

#include "graphics/Texture.h"
#include "util/Scene.h"

GLuint texture;

Scene scene;

void Game::update() {
    static bool look=false, first=true;
    static double lastX=0, lastY=0;
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS) {
        if (!look) {
            look=true; first=true;
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwRawMouseMotionSupported()) glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        double x,y; glfwGetCursorPos(window_,&x,&y);
        if (first) { lastX=x; lastY=y; first=false; }
        float dx=float(x-lastX), dy=float(y-lastY);
        lastX=x; lastY=y;
        game::cam.addLookDelta(-dx, dy);
    } else if (look) {
        look=false;
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwRawMouseMotionSupported()) glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    }

    if (Input::pressed(GLFW_KEY_ESCAPE))
        std::exit(0);
    float speed = (Input::down(GLFW_KEY_LEFT_SHIFT) ? 1.f : 10.f);
    float fwd = 0.f, str = 0.f;
    if (Input::down(GLFW_KEY_W))    fwd += speed;
    if (Input::down(GLFW_KEY_S))  fwd -= speed;
    if (Input::down(GLFW_KEY_D)) str -= speed;
    if (Input::down(GLFW_KEY_A))  str += speed;
    game::cam.moveRelativeXZ(fwd, str);
    scene.update();
}

void Game::draw() {
    // std::cout << whiteTexture << "\n";
    // glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 200.0f, -300.0f));
    // m = glm::scale(m, glm::vec3(400.0f, 1.0f, 100.0f));
    // if (!gQuad.vao || gQuad.indexCount==0) return;
    // game::shader.use();
    // game::shader.setMat4("uModel", m);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, whiteTexture);
    // game::shader.setInt("uTex", 0);
    // glBindVertexArray(gQuad.vao);
    // glDrawElements(GL_TRIANGLES, gQuad.indexCount, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);

    scene.draw();
}

int main() {
#ifdef ENABLE_AUTO_UPDATE
    if (SoftUpdateCheckAndMaybeRun()) {
        std::exit(0);
    }
#endif
    Game game;

    const std::string vertSrc = loadTextFile("resources/shaders/vertex/shader.vert");
    const std::string fragSrc = loadTextFile("resources/shaders/fragment/shader.frag");

    game::shader = {};
    if (!game::shader.loadFromSource(vertSrc.c_str(), fragSrc.c_str())) {
        throw std::runtime_error("Shader failed to compile/link");
    }

    GenWhiteTexture();

    initQuadMesh();
    std::cout << gQuad.indexCount << std::endl;

    texture = LoadTexture2D("resources/textures/block_sprite.png");
    scene = Scene();

    scene.setup();

    game.run();

    return 0;
}