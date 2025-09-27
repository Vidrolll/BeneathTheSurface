//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_CAMERA_H
#define BENEATHTHESURFACE_CAMERA_H

#include <vector>

#include <glm/trigonometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

// Camera.h
class Camera {
public:
    Camera& setPosition(const glm::vec3& p){ pos_=p; return *this; }
    Camera& setAngles(float yawDeg, float pitchDeg){
        yaw_=yawDeg; pitch_=glm::clamp(pitchDeg, -89.f, 89.f); return *this;
    }

    void addLookDelta(float dx, float dy, float sensitivity=0.12f) {
        // GLFW: +dx=mouse right, +dy=mouse down
        // Map: mouse right -> yaw right ; mouse up -> pitch up
        yaw_   += -dx * sensitivity;                 // yaw around Y
        pitch_  = glm::clamp(pitch_ - dy * sensitivity, -89.f, 89.f); // pitch around X
    }

    glm::vec3 forward() const {
        // Yaw around +Y, pitch around +X. Y-up, right-handed.
        float y = glm::radians(yaw_);
        float p = glm::radians(pitch_);
        return glm::normalize(glm::vec3(
            cosf(p) * cosf(y),   // X
            sinf(p),             // Y
            cosf(p) * sinf(y)    // Z
        ));
    }

    glm::vec3 right() const { return glm::normalize(glm::cross(forward(), {0.f,1.f,0.f})); }
    glm::vec3 up()    const { return glm::normalize(glm::cross(right(),   forward())); }

    // Move on your game's horizontal plane (X–Y plane; Z is depth)
    void moveRelativeXZ(float fwd, float strafe) {
        glm::vec3 f = forward();
        //f.y = 0.0f;                          // drop vertical
        if (glm::length(f) < 1e-6f) {
            // if looking straight up/down, fall back to yaw-only forward
            float y = glm::radians(yaw_);
            f = glm::vec3(cosf(y), 0.0f, sinf(y));
        }
        f = glm::normalize(f);

        // right vector on XZ plane
        glm::vec3 r = glm::normalize(glm::cross(glm::vec3(0,1,0), f));

        pos_ += f * fwd + r * strafe;
    }

    glm::mat4 getPerspectiveMat(float aspect) const {
        return glm::perspective(glm::radians(fov_), aspect, near_, far_);
    }
    glm::mat4 getViewMat() const {
        return glm::lookAt(pos_, pos_ + forward(), glm::vec3(0.f,1.f,0.f));
    }

private:
    glm::vec3 pos_{0.f,0.f,935.f};
    float yaw_   = -90.f;  // -90° makes forward initially point toward -Z
    float pitch_ =  15.f;  // slight tilt so “walls” are visible
    float fov_ = 60.f, near_ = 0.1f, far_ = -1.f;
};



#endif //BENEATHTHESURFACE_CAMERA_H