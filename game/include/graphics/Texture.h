//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_TEXTURE_H
#define BENEATHTHESURFACE_TEXTURE_H

#pragma once
#include <string>
#include <GL/glew.h>
#include <iostream>

inline GLuint whiteTexture;

void GenWhiteTexture();

GLuint LoadTexture2D(const std::string& path, bool srgb = false);
#endif //BENEATHTHESURFACE_TEXTURE_H