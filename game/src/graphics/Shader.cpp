//
// Created by Caden Vize on 9/23/2025.
//

// Shader.cpp

#include <iostream>
#include <GL/glew.h>
#include "graphics/Shader.h"

static bool compile(GLuint& out, GLenum type, const char* src) {
    out = glCreateShader(type);
    glShaderSource(out, 1, &src, nullptr);
    glCompileShader(out);
    GLint ok=0; glGetShaderiv(out, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[4096]; glGetShaderInfoLog(out,4096,nullptr,log);
        std::cerr<<"Shader compile error:\n"<<log<<"\n"; return false; }
    return true;
}

bool Shader::loadFromSource(const char* vsSrc, const char* fsSrc) {
    GLuint vs=0, fs=0;
    if(!compile(vs, GL_VERTEX_SHADER, vsSrc)) return false;
    if(!compile(fs, GL_FRAGMENT_SHADER, fsSrc)) return false;
    prog_ = glCreateProgram();
    glAttachShader(prog_, vs);
    glAttachShader(prog_, fs);
    glLinkProgram(prog_);
    glDeleteShader(vs); glDeleteShader(fs);
    GLint ok=0; glGetProgramiv(prog_, GL_LINK_STATUS, &ok);
    if(!ok){ char log[4096]; glGetProgramInfoLog(prog_,4096,nullptr,log);
        std::cerr<<"Link error:\n"<<log<<"\n"; return false; }
    return true;
}
void Shader::use() const { glUseProgram(prog_); }
int Shader::loc(const char* name) const { return glGetUniformLocation(prog_, name); }
void Shader::setMat4(const char* name, const glm::mat4& m) const { glUniformMatrix4fv(loc(name),1,GL_FALSE,&m[0][0]); }
void Shader::setInt(const char* name, int v) const { glUniform1i(loc(name), v); }
