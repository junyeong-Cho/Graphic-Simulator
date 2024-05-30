/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "CurveGeneration.hpp"

namespace graphics
{
    std::vector<glm::vec3> generateHermiteCurve(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& tangents, int segments)
    {
        std::vector<glm::vec3> curve;

        for (size_t i = 0; i < points.size() - 1; ++i)
        {
            for (int j = 0; j <= segments; ++j)
            {
                float t  = static_cast<float>(j) / static_cast<float>(segments);
                float t2 = t * t;
                float t3 = t2 * t;

                glm::vec3 p0 = points[i];
                glm::vec3 p1 = points[i + 1];
                glm::vec3 m0 = tangents[i];
                glm::vec3 m1 = tangents[i + 1];

                float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
                float h10 = t3 - 2.0f * t2 + t;
                float h01 = -2.0f * t3 + 3.0f * t2;
                float h11 = t3 - t2;

                glm::vec3 p = h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1;
                curve.push_back(p);
            }
        }

        return curve;
    }

    std::vector<glm::vec3> generateCatmullRomSpline(const std::vector<glm::vec3>& points, int segments)
    {
        std::vector<glm::vec3> curve;

        for (size_t i = 1; i < points.size() - 2; ++i)
        {
            glm::vec3 p0 = points[i - 1];
            glm::vec3 p1 = points[i];
            glm::vec3 p2 = points[i + 1];
            glm::vec3 p3 = points[i + 2];

            for (int j = 0; j <= segments; ++j)
            {
                float t  = static_cast<float>(j) / static_cast<float>(segments);
                float t2 = t * t;
                float t3 = t2 * t;

                glm::vec3 a = 2.0f * p1;
                glm::vec3 b = p2 - p0;
                glm::vec3 c = 2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3;
                glm::vec3 d = -p0 + 3.0f * p1 - 3.0f * p2 + p3;

                glm::vec3 p = 0.5f * (a + b * t + c * t2 + d * t3);
                curve.push_back(p);
            }
        }

        return curve;
    }
}
