//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_SCENE_H
#define BENEATHTHESURFACE_SCENE_H

#include <memory>
#include <vector>

#include "Tile.h"

static constexpr int UNIT_SIZE = 10;

class Scene {
public:
    Scene();

    void setup();

    void update();
    void draw();

    static void addTile(std::unique_ptr<Tile> tile) { tiles_.push_back(std::move(tile)); }
    static std::vector<std::unique_ptr<Tile>>& getTiles() { return tiles_; }
private:
    inline static std::vector<std::unique_ptr<Tile>> tiles_;
};

#endif //BENEATHTHESURFACE_SCENE_H