//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_TILE_H
#define BENEATHTHESURFACE_TILE_H

#include <memory>

#include "math/Polygon.h"
#include "graphics/Model.h"

class Tile {
public:
    Tile() = default;
    explicit Tile(std::unique_ptr<Polygon> hit, GLuint texture) : hit_(std::move(hit)), texture_(texture) {
        drawable_.init(hit_.get(),texture_,10);
    }
    explicit Tile(Polygon& p, GLuint texture) : Tile(std::make_unique<Polygon>(p), texture) {}
    explicit Tile(Polygon&& p, GLuint texture)      : Tile(std::make_unique<Polygon>(std::move(p)), texture) {}
    virtual ~Tile() = default;
    virtual void update() = 0;
    virtual void draw() = 0;

    const Polygon& getHit() const { return *hit_; }
    float getZ() const { return hit_->getZ(); }
protected:
    std::unique_ptr<Polygon> hit_;
    PolyDraw drawable_;
    GLuint texture_;
};

#endif //BENEATHTHESURFACE_TILE_H