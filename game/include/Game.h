//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_GAME_H
#define BENEATHTHESURFACE_GAME_H

#include "graphics/Window.h"

class Game final : public Window {
public:
    ~Game() override = default;
    void update() override;
    void draw() override;
    static GLFWwindow* getWindow() { return window_; }
};

#endif //BENEATHTHESURFACE_GAME_H