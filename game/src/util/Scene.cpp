//
// Created by Caden Vize on 9/21/2025.
//

#include "util/Scene.h"

#include <iostream>

#include "math/AABB.h"

#include <glm/vec2.hpp>

#include "graphics/Window.h"

#include "tiles/TestTile.h"
#include "math/Polygon.h"
#include "entites/Player.h"

inline Player player;

Scene::Scene() {
    for (int i = 0; i < 10; i++) {
        int x = i*10;
        std::vector<glm::vec3> vertices = {{x+0.f,0.f,0.f},{x+0.f,10.f,0.f},{x+10.f,10.f,0.f},{x+10.f,0.f,0.f}};
        Polygon hit_(std::move(vertices));
        tiles_.push_back(std::make_unique<TestTile>(hit_));
    }
}

void Scene::update() {
    for (auto& tile : tiles_) {
        tile->update();
    }
    player.update();
}

void drawViewRect(const AABB& r, float zLayer = 0.f);

void Scene::draw() {
    for (auto& tile : tiles_) {
        if (overlaps(tile->getHit().getBoundingRect(),
            gamecam::cam.getBoundingRect(tile->getZ())) ||
            overlaps(tile->getHit().getBoundingRect(),
            gamecam::cam.getBoundingRect(tile->getZ()-100.f))) {
            tile->draw();
        }
        drawViewRect(tile->getHit().getBoundingRect());
    }
    player.draw();
    drawViewRect(player.getHit().getBoundingRect());
}

void drawViewRect(const AABB& r, float zLayer) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(r.min_.x, r.min_.y, zLayer);
    glVertex3f(r.max_.x, r.min_.y, zLayer);
    glVertex3f(r.max_.x, r.max_.y, zLayer);
    glVertex3f(r.min_.x, r.max_.y, zLayer);
    glVertex3f(r.min_.x, r.min_.y, zLayer);
    glVertex3f(r.max_.x, r.max_.y, zLayer);
    glVertex3f(r.max_.x, r.min_.y, zLayer);
    glVertex3f(r.min_.x, r.max_.y, zLayer);
    glEnd();
}
