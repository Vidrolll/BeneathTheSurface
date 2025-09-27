//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_WINDOW_H
#define BENEATHTHESURFACE_WINDOW_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include "Camera.h"
#include "graphics/Shader.h"

struct game {
    static inline Camera cam;
    static inline Shader shader{};
};

class Window {
public:

    Window();

    virtual ~Window();
    void run();
    virtual void update() = 0;
    virtual void draw() = 0;
protected:
    inline static GLFWwindow* window_;
    const GLFWvidmode* mode_;
    GLFWmonitor* monitor_;
    int width_, height_;
    int fbw_, fbh_;

    void setupView(GLuint) const;
    static void cleanupView();
};

#endif //BENEATHTHESURFACE_WINDOW_H