//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_INPUT_H
#define BENEATHTHESURFACE_INPUT_H

#pragma once
#include <array>
#include <GLFW/glfw3.h>
#include "util/mouse_look.h"

class Input {
public:
    static void init(GLFWwindow* w) { sWindow = w; }

    static void cursorPosCallback(GLFWwindow* win, double x, double y) {
        if (!gLookActive) { gLastX = x; gLastY = y; return; }
        const double dx = x - gLastX;
        const double dy = y - gLastY;
        gLastX = x; gLastY = y;

        // ignore wild spikes (alt-tab etc.)
        if (fabs(dx) < 500.0 && fabs(dy) < 500.0) {
            gAccumDX += static_cast<float>(dx);
            gAccumDY += static_cast<float>(dy);
        }
    }

    // Call once per frame (before update): polls events and updates key buffers
    static void update() {
        glfwPollEvents();
        sPrev = sCurr;
        for (int k = 0; k <= GLFW_KEY_LAST; ++k)
            sCurr[k] = (sWindow && glfwGetKey(sWindow, k) == GLFW_PRESS) ? 1 : 0;

        // mouse buttons
        for (int b = 0; b <= GLFW_MOUSE_BUTTON_LAST; ++b)
            sMouseCurr[b] = (sWindow && glfwGetMouseButton(sWindow, b) == GLFW_PRESS) ? 1 : 0;

        // mouse position
        if (sWindow) glfwGetCursorPos(sWindow, &sMouseX, &sMouseY);
        sMousePrevX = sMouseX; sMousePrevY = sMouseY;
    }

    // Keyboard
    static bool down(int key)    { return sCurr[key]; }
    static bool pressed(int key) { return sCurr[key] && !sPrev[key]; }
    static bool released(int key){ return !sCurr[key] && sPrev[key]; }

    // Mouse
    static bool mouseDown(int btn)    { return sMouseCurr[btn]; }
    static bool mousePressed(int btn) { return sMouseCurr[btn] && !sMousePrev[btn]; }
    static bool mouseReleased(int btn){ return !sMouseCurr[btn] && sMousePrev[btn]; }
    static double mouseX() { return sMouseX; }
    static double mouseY() { return sMouseY; }

private:
    inline static GLFWwindow* sWindow = nullptr;

    inline static std::array<uint8_t, GLFW_KEY_LAST + 1> sCurr{};
    inline static std::array<uint8_t, GLFW_KEY_LAST + 1> sPrev{};

    inline static std::array<uint8_t, GLFW_MOUSE_BUTTON_LAST + 1> sMouseCurr{};
    inline static std::array<uint8_t, GLFW_MOUSE_BUTTON_LAST + 1> sMousePrev{};

    inline static double sMouseX = 0.0, sMouseY = 0.0;
    inline static double sMousePrevX = 0.0, sMousePrevY = 0.0;
};

#endif //BENEATHTHESURFACE_INPUT_H