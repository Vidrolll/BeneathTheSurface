//
// Created by Caden Vize on 9/21/2025.
//
#include "graphics/Window.h"
#include "util/Input.h"

#include <stdexcept>

Window::Window() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    monitor_ = glfwGetPrimaryMonitor();
    mode_ = glfwGetVideoMode(monitor_);

    width_ = mode_->width;
    height_ = mode_->height;

    window_ = glfwCreateWindow(width_, height_, "Beneath the Surface", monitor_, nullptr);
    if (!window_) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    glfwShowWindow(window_);

    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glViewport(0, 0, width_, height_);
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Window::run() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glOrtho(0, width_, height_, 0, -1, 1);
    Input::init(window_);
    while (!glfwWindowShouldClose(window_)) {
        glClear(GL_COLOR_BUFFER_BIT);
        Input::update();
        update();
        draw();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}
