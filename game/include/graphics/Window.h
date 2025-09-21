//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_WINDOW_H
#define BENEATHTHESURFACE_WINDOW_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

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
};

#endif //BENEATHTHESURFACE_WINDOW_H