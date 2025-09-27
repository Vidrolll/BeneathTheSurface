//
// Created by Caden Vize on 9/23/2025.
//

#ifndef BENEATHTHESURFACE_MOUSE_LOOK_H
#define BENEATHTHESURFACE_MOUSE_LOOK_H

#pragma once
#include <GLFW/glfw3.h>
#include "graphics/Window.h"

inline bool gLookActive;
inline double gLastX, gLastY;
inline float gAccumDX, gAccumDY;

inline void cursorPosCallback(GLFWwindow* win, double x, double y) {
    if (!gLookActive) { gLastX = x; gLastY = y; return; }
    double dx = x - gLastX, dy = y - gLastY;
    gLastX = x; gLastY = y;
    if (std::abs(dx) < 500.0 && std::abs(dy) < 500.0) {
        gAccumDX += (float)dx;
        gAccumDY += (float)dy;
    }
}
inline void handleMouseButtons(GLFWwindow* window) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !gLookActive) {
        gLookActive = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        double x,y; glfwGetCursorPos(window, &x, &y);
        gLastX = x; gLastY = y;
        gAccumDX = gAccumDY = 0.0f;
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && gLookActive) {
        gLookActive = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    }
}
inline void updateCameraFromMouse() {
    if (!gLookActive) return;
    float dx = gAccumDX, dy = gAccumDY;   // read…
    gAccumDX = gAccumDY = 0.0f;           // …then zero (this is correct)
    if (dx == 0.f && dy == 0.f) return;

    constexpr float sens = 0.12f;
    // map mouse right→yaw right, mouse up→pitch up (GLFW y+ is down)
    constexpr float YAW_SIGN = -1.f, PITCH_SIGN = -1.f;
    game::cam.addLookDelta(YAW_SIGN * dx, PITCH_SIGN * dy, sens);
}

#endif //BENEATHTHESURFACE_MOUSE_LOOK_H