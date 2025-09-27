//
// Created by Caden Vize on 9/21/2025.
//

#include "graphics/Window.h"

#include <iostream>
#include <chrono>

#include "util/Input.h"

#include <stdexcept>

#include <glm/trigonometric.hpp>
#include <glm/mat4x4.hpp>

#include <glm/ext/matrix_clip_space.hpp>

#include "Game.h"
#include "util/Time.h"

Window::Window() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

    monitor_ = glfwGetPrimaryMonitor();
    mode_ = glfwGetVideoMode(monitor_);

    width_ = mode_->width;
    height_ = mode_->height;

    window_ = glfwCreateWindow(width_, height_, "Beneath the Surface", nullptr, nullptr);
    if (!window_) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    glfwShowWindow(window_);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glfwSetCursorPosCallback(window_, Input::cursorPosCallback);

    glfwGetFramebufferSize(window_, &fbw_, &fbh_);
    glViewport(0, 0, fbw_, fbh_);
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Window::setupView(const GLuint shaderId) const {
    glUseProgram(shaderId);
    glm::mat4 proj = game::cam.getPerspectiveMat(static_cast<float>(width_)/static_cast<float>(height_));
    glm::mat4 view = game::cam.getViewMat();
    glUniformMatrix4fv(glGetUniformLocation(shaderId,"uProj"),1,GL_FALSE,&proj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderId,"uView"),1,GL_FALSE,&view[0][0]);
}

void Window::run() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    Input::init(window_);
    using clock = std::chrono::high_resolution_clock;
    Time::lastTime = clock::now();
    while (!glfwWindowShouldClose(window_)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto now = clock::now();
        const auto delta = std::chrono::duration<float>(now - Time::lastTime).count();
        Time::updateLastTime(now);
        Time::updateDeltaTime(delta);

        Input::update();
        update();
        setupView(game::shader.id());
        draw();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}
