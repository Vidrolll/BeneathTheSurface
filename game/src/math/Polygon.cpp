//
// Created by Caden Vize on 9/21/2025.
//

#include "math/Polygon.h"

#include <algorithm>
#include "util/Scene.h"
#include <iostream>

Polygon::Polygon(std::vector<glm::vec3> v, Tag)
       : vertices_(std::move(v)), transform_(1.0f) {
    directVertices();
    float minX = vertices_[0].x, maxX = vertices_[0].x;
    float minY = vertices_[0].y, maxY = vertices_[0].y;
    for (const auto& vert : vertices_) {
        minX = std::min(minX, vert.x);
        maxX = std::max(maxX, vert.x);
        minY = std::min(minY, vert.y);
        maxY = std::max(maxY, vert.y);
    }
    boundingRect_ = {{minX, minY}, {maxX, maxY}};
}

std::vector<glm::vec2> Polygon::getBoundingRect() const {
    std::vector<glm::vec2> box = boundingRect_;
    box[0] = transform_ * glm::vec4(boundingRect_[0],0.f,1.f);
    box[1] = transform_ * glm::vec4(boundingRect_[1],0.f,1.f);
    box[0] *= UNIT_SIZE;
    box[1] *= UNIT_SIZE;
    return box;
}

Polygon::Polygon(const std::vector<glm::vec3>& vertices)
        : Polygon(vertices, Tag{}) {}
Polygon::Polygon(std::vector<glm::vec3>&& vertices)
        : Polygon(std::move(vertices), Tag{}) {}

void Polygon::directVertices() {
    if (vertices_.size() < 3) return;
    float sum = 0;
    for (int i = 0; i < vertices_.size(); ++i) {
        sum += cross(vertices_[i], vertices_[(i+1) % vertices_.size()]);
    }
    if (sum < 0) {
        std::ranges::reverse(vertices_);
    }
}

std::vector<glm::vec3> Polygon::getTransformedVertices() const {
    std::vector<glm::vec3> vertices = vertices_;
    for (auto& v : vertices) {
        v = transform_ * glm::vec4(v, 1.0f);
    }
    return vertices;
}

glm::vec3 Polygon::getPos() const {
    return {transform_[3][0],transform_[3][1],transform_[3][2]};
}

void Polygon::move(const glm::vec3 &offset) {
    transform_ = glm::translate(transform_, offset);
}

/**
 * @param angle The angle (in radians) to rotate the transform matrix by.\n
 * (Rotates counterclockwise)
 */
void Polygon::rotate(const float angle) {
    transform_ = glm::rotate(transform_, angle, {0,0,1});
}

void Polygon::scale(const glm::vec3 &scale) {
    transform_ = glm::scale(transform_, scale);
}