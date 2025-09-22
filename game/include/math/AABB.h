//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_AABB_H
#define BENEATHTHESURFACE_AABB_H
#include <glm/vec2.hpp>

#include <vector>
#include <initializer_list>
#include <stdexcept>

struct AABB {
    glm::vec2 min_{0.0f};
    glm::vec2 max_{0.0f};

    AABB() = default;
    AABB(glm::vec2 mn, glm::vec2 mx) : min_(mn), max_(mx) { normalize_(); }

    // lets you write AABB box{{{0,0},{10,5}}} and also pass {{...}} to a function
    AABB(std::initializer_list<glm::vec2> il) {
        if (il.size() != 2) throw std::runtime_error("AABB needs exactly 2 points");
        auto it = il.begin();
        min_ = *it++;
        max_ = *it;
        normalize_();
    }

    // (optional) build from any vector/span of points (computes bounds of all)
    AABB(const std::vector<glm::vec2>& pts) {
        if (pts.empty()) { min_ = max_ = {}; return; }
        min_ = max_ = pts[0];
        for (auto& p : pts) {
            min_.x = std::min(min_.x, p.x); min_.y = std::min(min_.y, p.y);
            max_.x = std::max(max_.x, p.x); max_.y = std::max(max_.y, p.y);
        }
    }

private:
    void normalize_() {
        if (min_.x > max_.x) std::swap(min_.x, max_.x);
        if (min_.y > max_.y) std::swap(min_.y, max_.y);
    }
};

inline bool overlaps(const AABB& a, const AABB& b) {
    return !(a.max_.x < b.min_.x || a.min_.x > b.max_.x ||
             a.max_.y < b.min_.y || a.min_.y > b.max_.y);
}

#endif //BENEATHTHESURFACE_AABB_H