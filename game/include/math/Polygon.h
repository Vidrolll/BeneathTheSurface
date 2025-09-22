//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_POLYGON_H
#define BENEATHTHESURFACE_POLYGON_H

#include <vector>
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Polygon {
public:
    explicit Polygon(const std::vector<glm::vec3>& vertices);
    explicit Polygon(std::vector<glm::vec3>&& vertices);

    glm::vec3 getPos() const;
    void move(const glm::vec3& offset);
    void rotate(float angle);
    void scale(const glm::vec3& scale);

    std::vector<glm::vec2> getBoundingRect() const;
    const std::vector<glm::vec3>& getVertices() const { return vertices_; }
    const glm::mat4& getTransform() const { return transform_; }

    std::vector<glm::vec3> getTransformedVertices() const;

    void setTransform(const glm::mat4& transform) { transform_ = transform; }
    void setVertices(const std::vector<glm::vec3>& vertices) { vertices_ = vertices; }
    void setVertices(std::vector<glm::vec3>&& vertices) { vertices_ = std::move(vertices); }

    float getZ() const { return getPos().z; };
private:
    struct Tag {};

    std::vector<glm::vec3> vertices_;
    glm::mat4 transform_;

    std::vector<glm::vec2> boundingRect_;

    Polygon(std::vector<glm::vec3> v, Tag);

    static float cross(const glm::vec3& a, const glm::vec3& b) {
        return a.x * b.y - a.y * b.x;
    }

    void directVertices();
};

#endif //BENEATHTHESURFACE_POLYGON_H