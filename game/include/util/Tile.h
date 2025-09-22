//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_TILE_H
#define BENEATHTHESURFACE_TILE_H

#include <memory>

#include "math/Polygon.h"

class Tile {
public:
    Tile() = default;
    explicit Tile(std::unique_ptr<const Polygon> hit) : hit_(std::move(hit)) {}
    explicit Tile(const Polygon& p) : Tile(std::make_unique<Polygon>(p)) {}
    explicit Tile(Polygon&& p)      : Tile(std::make_unique<Polygon>(std::move(p))) {}
    virtual ~Tile() = default;
    virtual void update() = 0;
    virtual void draw() = 0;

    const Polygon& getHit() const { return *hit_; }
    float getZ() const { return hit_->getZ(); }
protected:
    std::unique_ptr<const Polygon> hit_;
};

#endif //BENEATHTHESURFACE_TILE_H