//
// Created by Caden Vize on 9/21/2025.
//

#include "tiles/TestTile.h"

#include <iostream>

#include "util/Scene.h"

#include <GL/glew.h>

void TestTile::update() {

}
// VAO for unit quad (two triangles) must already exist: quadVao, quadIndexCount
// Shader must already have uProj/uView set for this frame.

static void drawEdgeWall(const Polygon& poly,
                         const glm::vec3& v1_local,
                         const glm::vec3& v2_local,
                         float depth_world,       // e.g. 100.f
                         GLuint shaderId,
                         GLuint quadVao, GLsizei quadIndexCount,
                         GLuint tex)             // gWhiteTex or a wall texture
{
    // Edge in *local* polygon space (XY).
    const glm::vec2 p1(v1_local.x, v1_local.y);
    const glm::vec2 p2(v2_local.x, v2_local.y);
    const glm::vec2 e  = p2 - p1;
    const float     L_local = glm::length(e);
    if (L_local <= 1e-6f) return;

    const float angleZ = std::atan2(e.y, e.x);

    // We build the wall entirely in polygon-local space, then transform to world:
    // world = unitScale * polyModel * translate(v1_local) * rotZ(angle) * rotX(-90°) * scale(L_local, depth_local, 1)
    // (depth_local = depth_world / UNIT_SIZE, since unitScale will convert to world)
    const float depth_local = depth_world / UNIT_SIZE;

    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(v1_local.x, v1_local.y, v1_local.z));
    model = glm::rotate(model, angleZ, glm::vec3(0,0,1));            // align +X with edge
    model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1,0,0)); // +Y -> -Z  (NOTE the minus)
    model = glm::scale(model, glm::vec3(L_local, depth_local, 1.f));

    // Prepend polygon's model and the global UNIT scale
    glm::mat4 unitScale = glm::scale(glm::mat4(1.f), glm::vec3(UNIT_SIZE));
    glm::mat4 fullModel = unitScale * poly.getTransform() * model;

    glUseProgram(shaderId);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "uModel"), 1, GL_FALSE, &fullModel[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(shaderId, "uTex"), 0);

    // If culling is on, some walls may vanish depending on winding; disable for now
    GLboolean cullWasEnabled = glIsEnabled(GL_CULL_FACE);
    if (cullWasEnabled) glDisable(GL_CULL_FACE);

    glBindVertexArray(quadVao);
    glDrawElements(GL_TRIANGLES, quadIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (cullWasEnabled) glEnable(GL_CULL_FACE);
}
void TestTile::draw() {
    // glColor3f(1.0f, 1.0f, 1.0f);
    // glEnable(GL_TEXTURE_2D);
    // glBegin(GL_POLYGON);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // const std::vector<glm::vec3> vertices = hit_->getTransformedVertices();
    // for (int i = 0; i < vertices.size(); i++) {
    //     constexpr glm::vec2 texCoords[] = {
    //         {0,0},{1,0},{1,1},{0,1}
    //     };
    //     const glm::vec3 v = vertices[i];
    //     glTexCoord2i(texCoords[i].x, texCoords[i].y);
    //     glVertex2f(v.x * UNIT_SIZE, v.y * UNIT_SIZE);
    // }
    // glEnd();
    // glDisable(GL_TEXTURE_2D);
    // for (int i = 0; i < vertices.size(); i++) {
    //     glm::vec3 v1 = vertices[i];
    //     glm::vec3 v2 = vertices[(i+1) % vertices.size()];
    //     if (v1.x==v2.x)
    //         glColor3f(1.0f, 0.0f, 0.0f);
    //     else
    //         glColor3f(0.0f, 1.0f, 0.0f);
    //     glBegin(GL_POLYGON);
    //     glVertex3f(v1.x * UNIT_SIZE, v1.y * UNIT_SIZE, v1.z * UNIT_SIZE);
    //     glVertex3f(v1.x * UNIT_SIZE, v1.y * UNIT_SIZE, v1.z * UNIT_SIZE-100.f);
    //     glVertex3f(v2.x * UNIT_SIZE, v2.y * UNIT_SIZE, v1.z * UNIT_SIZE-100.f);
    //     glVertex3f(v2.x * UNIT_SIZE, v2.y * UNIT_SIZE, v1.z * UNIT_SIZE);
    //     glEnd();
    // }
    drawable_.draw(game::shader);

    const auto& lv = hit_->getVertices();
    for (size_t i = 0; i < lv.size(); ++i) {
        const glm::vec3 v1 = lv[i];
        const glm::vec3 v2 = lv[(i+1) % lv.size()];
        drawEdgeWall(*hit_, v1, v2,
                     /*depth_world*/ 100.f,
                     game::shader.id(), gQuad.vao, gQuad.indexCount,
                     whiteTexture /* or a wall texture */);
    }
}