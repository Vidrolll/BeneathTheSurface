//
// Created by Caden Vize on 9/21/2025.
//

#include "tiles/TestTile.h"
#include "util/Scene.h"

#include <GL/glew.h>

TestTile::TestTile() {

}


void TestTile::update() {

}

void TestTile::draw() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
    glBindTexture(GL_TEXTURE_2D, texture);
    const std::vector<glm::vec3> vertices = hit_->getTransformedVertices();
    for (int i = 0; i < vertices.size(); i++) {
        constexpr glm::vec2 texCoords[] = {
            {0,0},{1,0},{1,1},{0,1}
        };
        const glm::vec3 v = vertices[i];
        glTexCoord2i(texCoords[i].x, texCoords[i].y);
        glVertex2f(v.x * UNIT_SIZE, v.y * UNIT_SIZE);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    for (int i = 0; i < vertices.size(); i++) {
        glm::vec3 v1 = vertices[i];
        glm::vec3 v2 = vertices[(i+1) % vertices.size()];
        if (v1.x==v2.x)
            glColor3f(1.0f, 0.0f, 0.0f);
        else
            glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_POLYGON);
        glVertex3f(v1.x * UNIT_SIZE, v1.y * UNIT_SIZE, v1.z * UNIT_SIZE);
        glVertex3f(v1.x * UNIT_SIZE, v1.y * UNIT_SIZE, v1.z * UNIT_SIZE-100.f);
        glVertex3f(v2.x * UNIT_SIZE, v2.y * UNIT_SIZE, v1.z * UNIT_SIZE-100.f);
        glVertex3f(v2.x * UNIT_SIZE, v2.y * UNIT_SIZE, v1.z * UNIT_SIZE);
        glEnd();
    }
}