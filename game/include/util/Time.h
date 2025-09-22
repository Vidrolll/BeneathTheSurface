//
// Created by Caden Vize on 9/22/2025.
//

#ifndef BENEATHTHESURFACE_TIME_H
#define BENEATHTHESURFACE_TIME_H

#pragma once

#include <chrono>

struct Time {
    static inline float deltaTime;
    static inline float tickSpeed = 5.f;

    static inline std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

    static void updateDeltaTime(const float dt) { deltaTime = dt * tickSpeed; }
    static void updateLastTime(const std::chrono::high_resolution_clock::time_point time) { lastTime = time; }
};

#endif //BENEATHTHESURFACE_TIME_H