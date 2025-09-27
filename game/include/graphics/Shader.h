//
// Created by Caden Vize on 9/23/2025.
//

#ifndef BENEATHTHESURFACE_SHADER_H
#define BENEATHTHESURFACE_SHADER_H
#include <GL/glew.h>

// Shader.h
#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <glm/glm.hpp>

inline std::string loadTextFile(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

class Shader {
public:
    Shader() = default;
    bool loadFromSource(const char* vsSrc, const char* fsSrc);
    void use() const;
    [[nodiscard]] unsigned id() const { return prog_; }

    // tiny helpers
    int loc(const char* name) const;
    void setMat4(const char* name, const glm::mat4& m) const;
    void setInt(const char* name, int v) const;

private:
    unsigned prog_ = 0;
};


#endif //BENEATHTHESURFACE_SHADER_H