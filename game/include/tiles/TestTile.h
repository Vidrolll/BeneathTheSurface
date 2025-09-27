//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_TESTTILE_H
#define BENEATHTHESURFACE_TESTTILE_H

#include "util/Tile.h"
#include "graphics/Window.h"

class TestTile final : public Tile {
    using Tile::Tile;
    void update() override;
    void draw() override;
};

#endif //BENEATHTHESURFACE_TESTTILE_H