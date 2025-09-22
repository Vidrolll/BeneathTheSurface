//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_PLAYER_H
#define BENEATHTHESURFACE_PLAYER_H
#include "util/Entity.h"

class Player : public Entity {
public:
    using Entity::Entity;

    Player();

    void update() override;
    void draw() override;
    void collisionCheck();
};

#endif //BENEATHTHESURFACE_PLAYER_H