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

class Camera {
public:
    std::vector<glm::vec2> getBoundingRect(float z) const {
        const float HL = 2 * (pos_.z - z) * tan(glm::radians(fov_ / 2.f));
        const float WL = HL * aspect_;
        return {{pos_.x-WL/2, pos_.y-HL/2}, {pos_.x+WL/2, pos_.y+HL/2}};
    }

    glm::mat4 getPerspectiveMat(float aspect) {
        aspect_ = aspect;
        return glm::perspective(glm::radians(fov_), aspect_, near_, far_);
    }

    glm::mat4 getViewMat()  const {
        const glm::vec3 eye = {pos_.x,pos_.y,pos_.z};
        const glm::vec3 target = {pos_.x,pos_.y,0.f};
        constexpr glm::vec3 up = {0.f, 1.f, 0.f};
        return glm::lookAt(eye, target, up);
    };

    void move(const glm::vec3& offset) {
        pos_ += offset;
    }
private:
    glm::vec3 pos_{0.f,0.f,935.f};
    float fov_ = 60.f;
    float near_ = 0.1f;
    float far_ = 5000.f;
    float aspect_ = 0;
};

#endif //BENEATHTHESURFACE_CAMERA_H