//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_ENTITY_H
#define BENEATHTHESURFACE_ENTITY_H

#include <memory>

#include "math/Polygon.h"

class Entity {
public:
    Entity() = default;
    explicit Entity(std::unique_ptr<Polygon> hit) : hit_(std::move(hit)) {}
    explicit Entity(Polygon& p) : Entity(std::make_unique<Polygon>(p)) {}
    explicit Entity(Polygon&& p)      : Entity(std::make_unique<Polygon>(std::move(p))) {}
    virtual ~Entity() = default;
    virtual void update() = 0;
    virtual void draw() = 0;

    void move(const glm::vec3 offset) const { hit_->move(offset); }

    const glm::vec3& getVel() const { return vel_; }
    void setVel(const glm::vec3& vel) { vel_ = vel; }
    const Polygon& getHit() const { return *hit_; }
    float getZ() const { return hit_->getZ(); }
protected:
    std::unique_ptr<Polygon> hit_;
    glm::vec3 vel_;
};

#endif //BENEATHTHESURFACE_ENTITY_H