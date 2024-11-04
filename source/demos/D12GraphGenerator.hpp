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
    class D12GraphGenerator : public IDemo
    {
    public:
        D12GraphGenerator();

        void Update() override;
        void Draw() const override;
        void ImGuiDraw() override;

        constexpr void SetDisplaySize([[maybe_unused]] int width, [[maybe_unused]] int height) override
        {
        }

    private:
        void UpdateGraph();
        void HandleInput();

        GLShader      shader;
        GLVertexArray graphMesh;

        assets::Reloader assetReloader;

        std::vector<graphics::MeshVertex> graphVertices;
        std::vector<unsigned>             graphIndices;

        GLAttributeLayout position;
        GLAttributeLayout color;
        GLAttributeLayout uv;

        int   samples;
        float minValue;
        float maxValue;
        float slope;
        float xStart;
        float xEnd;
        float zoomLevel;
    };
}
