//
// Created by Caden Vize on 9/21/2025.
//

#include "entites/Player.h"

#include <iostream>
#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

#include "util/Scene.h"

#include "util/Input.h"
#include "math/AABB.h"
#include "math/SAT.h"

#include "util/Time.h"

Player::Player() {
    std::vector<glm::vec3> vertices = {{0.f,0.f,0.f},{0.f,20.f,0.f},{10.f,20.f,0.f},{10.f,0.f,0.f}};
    hit_ = std::make_unique<Polygon>(std::move(vertices));
    hit_->move({20,50,0});
}


void Player::update() {
    vel_.x = 0;
    const float speed = (Input::down(GLFW_KEY_LEFT_SHIFT) ? 2.5f : 2.f);
    if (Input::down(GLFW_KEY_A))
        vel_ += glm::vec3(-speed,0,0);
    if (Input::down(GLFW_KEY_D))
        vel_ += glm::vec3(speed,0,0);
    if (Input::pressed(GLFW_KEY_SPACE)) vel_.y = 3;
    vel_ -= glm::vec3(0.f,Time::deltaTime,0.f);
    move(vel_ * Time::deltaTime);
    collisionCheck();
}

void Player::draw() {
    glColor3f(0.f, 0.f, 1.0f);
    glBegin(GL_POLYGON);
    const std::vector<glm::vec3> vertices = hit_->getTransformedVertices();
    for (auto& v : vertices) {
        glVertex3f(v.x * UNIT_SIZE, v.y * UNIT_SIZE, -25.f);
    }
    glEnd();
}

void Player::collisionCheck() {
    for (auto& t : Scene::getTiles()) {
        if (!overlaps(t->getHit().getBoundingRect(),hit_->getBoundingRect()))
            continue;
        if (glm::vec3 displacement; sat(hit_->getTransformedVertices(), t->getHit().getTransformedVertices(), displacement)) {
            move(displacement);
            vel_.y = 0;
        }
    }
}