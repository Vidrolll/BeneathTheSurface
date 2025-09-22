//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_SAT_H
#define BENEATHTHESURFACE_SAT_H

#include <vector>
#include <glm/vec2.hpp>
#include <algorithm>
#include <glm/glm.hpp>
#include <limits>
#include <cmath>

inline bool sat(const std::vector<glm::vec3>& A,
                       const std::vector<glm::vec3>& B,
                       glm::vec3& out)
{
    float minimumTranslation = std::numeric_limits<float>::max();
    glm::vec2 minimumTranslationVector = glm::vec2(minimumTranslation,minimumTranslation);
    auto V = [&](const std::vector<glm::vec3>& vA, const std::vector<glm::vec3>& vB, float correction) -> bool {
        for (int i = 0; i < vA.size(); i++) {
            const glm::vec2 sE = vA[i] - vA[(i + 1) % vA.size()];
            const glm::vec2 nE = normalize(glm::vec2(-sE.y,sE.x));
            float aMin = std::numeric_limits<float>::max(),
                  aMax = std::numeric_limits<float>::min(),
                  bMin = std::numeric_limits<float>::max(),
                  bMax = std::numeric_limits<float>::min();
            for (auto& v : vA) {
                const float dp = dot(nE, glm::vec2(v.x,v.y));
                if (dp < aMin) aMin = dp;
                if (dp > aMax) aMax = dp;
            }
            for (auto& v : vB) {
                const float dp = dot(nE, glm::vec2(v.x,v.y));
                if (dp < bMin) bMin = dp;
                if (dp > bMax) bMax = dp;
            }
            if (aMax < bMin || bMax < aMin) return false;
            const float distance = (aMax - bMin) * correction;
            if (length(nE * -distance) < length(minimumTranslationVector))
                minimumTranslationVector = nE * -distance;
            if (distance < minimumTranslation) minimumTranslation = distance;
        }
        out.x = minimumTranslationVector.x;
        out.y = minimumTranslationVector.y;
        return true;
    };
    return V(A,B,1) && V(B,A,-1);
}



#endif //BENEATHTHESURFACE_SAT_H