/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "IDemo.hpp"
#include "assets/Reloader.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "opengl/GLFrameBuffer.hpp"
#include "opengl/GLShader.hpp"
#include "opengl/GLTexture.hpp"
#include <array>
#include <glm/vec3.hpp>
#include <vector>

namespace demos
{
    enum class CurveType
    {
        Hermite,
        Catmull,
        All
    };

    class D11CurvesNSplines : public IDemo
    {
    public:
        D11CurvesNSplines();

        void Update() override;
        void Draw() const override;
        void ImGuiDraw() override;

        constexpr void SetDisplaySize([[maybe_unused]] int width, [[maybe_unused]] int height) override
        {
        }

    private:
        void UpdateCurves();
        void HandleInput();

        GLShader      shader;
        GLVertexArray hermiteMesh;
        GLVertexArray catmullMesh;
        GLVertexArray circleMesh;
        GLVertexArray tangentMesh;

        assets::Reloader assetReloader;

        std::vector<glm::vec3> controlPoints;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> initialControlPoints;
        std::vector<glm::vec3> initialTangents;

        std::vector<graphics::MeshVertex> hermiteVertices;
        std::vector<unsigned>             hermiteIndices;

        std::vector<graphics::MeshVertex> catmullVertices;
        std::vector<unsigned>             catmullIndices;

        std::vector<graphics::MeshVertex> tangentVertices;
        std::vector<unsigned>             tangentIndices;

        GLAttributeLayout position;
        GLAttributeLayout color;
        GLAttributeLayout uv;

        int       selectedPointIndex;
        CurveType selectedCurveType;

        int   samples;
        float tangentLength;
        float zoomLevel;
    };
}
