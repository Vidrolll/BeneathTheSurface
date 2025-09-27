//
// Created by Caden Vize on 9/23/2025.
//

#ifndef BENEATHTHESURFACE_MODEL_H
#define BENEATHTHESURFACE_MODEL_H

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <numeric>
#include <cmath>
#include <cfloat>

#include "math/Polygon.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"

struct Mesh {
    GLuint vao=0, vbo=0, ebo=0;
    GLsizei indexCount=0;
    void destroy() {
        if (ebo)
            glDeleteBuffers(1,&ebo);
        if (vbo)
            glDeleteBuffers(1,&vbo);
        if (vao)
            glDeleteVertexArrays(1,&vao);
        vao=vbo=ebo=0;
        indexCount=0;
    }
};

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::u8vec4 color;
};

static inline const Vertex QUAD_VERTS[4] = {
    {{0,0,0}, {0,0}, {255,255,255,255}},
    {{1,0,0}, {1,0}, {255,255,255,255}},
    {{1,1,0}, {1,1}, {255,255,255,255}},
    {{0,1,0}, {0,1}, {255,255,255,255}},
};
static inline constexpr uint32_t QUAD_IDX[6] = {0,1,2, 0,2,3};

struct QuadMesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;
};

inline QuadMesh gQuad;

inline void initQuadMesh() {
    glGenVertexArrays(1, &gQuad.vao);
    glBindVertexArray(gQuad.vao);

    glGenBuffers(1, &gQuad.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gQuad.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTS), QUAD_VERTS, GL_STATIC_DRAW);

    glGenBuffers(1, &gQuad.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gQuad.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_IDX), QUAD_IDX, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,uv));
    glEnableVertexAttribArray(2); // color (normalized u8)
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex,color));

    glBindVertexArray(0);
    gQuad.indexCount = 6;
}

// Simple UVs: project XY into [0,1] by polygon AABB (local space)
static inline void makeUVsFromLocalAABB(const std::vector<glm::vec3>& verts, std::vector<glm::vec2>& uvs) {
    glm::vec2 mn(FLT_MAX), mx(-FLT_MAX);
    for (auto& v: verts) { mn = glm::min(mn, glm::vec2(v)); mx = glm::max(mx, glm::vec2(v)); }
    glm::vec2 sz = glm::max(mx-mn, glm::vec2(1e-6f));
    uvs.resize(verts.size());
    for (size_t i=0;i<verts.size();++i) {
        glm::vec2 p = glm::vec2(verts[i]);
        uvs[i] = (p - mn) / sz;
    }
}

// Convex: triangle fan indices 0,(i),(i+1)
static inline void makeFanIndices(size_t n, std::vector<uint32_t>& idx) {
    idx.clear();
    if (n < 3) return;
    idx.reserve((n-2)*3);
    for (uint32_t i=1;i+1<n;++i) { idx.push_back(0); idx.push_back(i); idx.push_back(i+1); }
}

// Minimal ear-clipping for concave simple polygons (CCW)
static bool triangulateEarClip(const std::vector<glm::vec3>& v, std::vector<uint32_t>& out) {
    const int n = (int)v.size(); if (n<3) return false;
    std::vector<int> idx(n); std::iota(idx.begin(), idx.end(), 0);

    auto area2 = [&](int a,int b,int c){
        return (v[b].x - v[a].x)*(v[c].y - v[a].y) - (v[b].y - v[a].y)*(v[c].x - v[a].x);
    };
    auto pointInTri = [&](glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c){
        float a1 = (b.x-a.x)*(p.y-a.y) - (b.y-a.y)*(p.x-a.x);
        float a2 = (c.x-b.x)*(p.y-b.y) - (c.y-b.y)*(p.x-b.x);
        float a3 = (a.x-c.x)*(p.y-c.y) - (a.y-c.y)*(p.x-c.x);
        bool hasNeg = (a1<0)||(a2<0)||(a3<0), hasPos=(a1>0)||(a2>0)||(a3>0);
        return !(hasNeg && hasPos);
    };

    out.clear(); out.reserve((n-2)*3);
    int guard = 0;
    while ((int)idx.size() > 3 && guard < 10000) {
        ++guard;
        bool clipped = false;
        for (int ii=0; ii<(int)idx.size(); ++ii) {
            int i0 = idx[(ii + idx.size()-1)%idx.size()];
            int i1 = idx[ii];
            int i2 = idx[(ii + 1)%idx.size()];
            if (area2(i0,i1,i2) <= 0) continue; // not CCW ear

            bool anyInside=false;
            glm::vec2 a(v[i0]), b(v[i1]), c(v[i2]);
            for (int k=0;k<(int)idx.size();++k) {
                int ik = idx[k]; if (ik==i0 || ik==i1 || ik==i2) continue;
                if (pointInTri(glm::vec2(v[ik]), a,b,c)) { anyInside=true; break; }
            }
            if (anyInside) continue;

            // ear!
            out.push_back((uint32_t)i0);
            out.push_back((uint32_t)i1);
            out.push_back((uint32_t)i2);
            idx.erase(idx.begin()+ii);
            clipped = true;
            break;
        }
        if (!clipped) break; // degenerate; stop
    }
    if (idx.size()==3) { out.push_back(idx[0]); out.push_back(idx[1]); out.push_back(idx[2]); }
    return out.size() >= 3;
}

static Mesh buildMeshFromPolygon(const Polygon& poly, glm::u8vec4 rgba = {255,255,255,255}) {
    Mesh m;
    const auto& verts = poly.getVertices();
    if (verts.size() < 3) return m;

    // Build vertex array
    std::vector<glm::vec2> uvs; makeUVsFromLocalAABB(verts, uvs);
    std::vector<Vertex> vboData(verts.size());
    for (size_t i=0;i<verts.size();++i) vboData[i] = { verts[i], uvs[i], rgba };

    // Indices: try triangle fan (fast) else ear-clip
    std::vector<uint32_t> idx;
    // If you know it's convex thanks to your winding check, fan is fine:
    makeFanIndices(verts.size(), idx);
    if (idx.empty()) triangulateEarClip(verts, idx);

    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, vboData.size()*sizeof(Vertex), vboData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(uint32_t), idx.data(), GL_STATIC_DRAW);
    m.indexCount = (GLsizei)idx.size();

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,pos));
    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,uv));
    glEnableVertexAttribArray(2); // color
    glVertexAttribPointer(2,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(Vertex),(void*)offsetof(Vertex,color));

    glBindVertexArray(0);
    return m;
}

struct PolyDraw {
    const Polygon* poly = nullptr;
    Mesh mesh;
    GLuint tex = 0;            // 0 => use gWhiteTex
    glm::mat4 unitScale = glm::mat4(1.f);

    void init(const Polygon* p, const GLuint texture /*0 ok*/, float unitSize) {
        poly = p; tex = texture;
        mesh = buildMeshFromPolygon(*poly);
        unitScale = glm::scale(glm::mat4(1.f), glm::vec3(unitSize));
    }
    void destroy(){ mesh.destroy(); }

    void draw(const Shader& sh) const {
        if (!mesh.vao || mesh.indexCount==0) return;
        sh.use();
        const glm::mat4 model = unitScale * poly->getTransform();
        sh.setMat4("uModel", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex ? tex : whiteTexture);
        sh.setInt("uTex", 0);
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};


#endif //BENEATHTHESURFACE_MODEL_H