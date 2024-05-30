/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace graphics
{
    std::vector<glm::vec3> generateHermiteCurve(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& tangents, int segments);
    std::vector<glm::vec3> generateCatmullRomSpline(const std::vector<glm::vec3>& points, int segments);
}
