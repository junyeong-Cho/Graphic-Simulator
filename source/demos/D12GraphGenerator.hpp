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
        void InitializeCircleMesh();
        void SaveGraphValuesToFile();


        GLShader      shader;
        GLVertexArray graphMesh;
        GLVertexArray circleMesh;

        assets::Reloader assetReloader;

        std::vector<graphics::MeshVertex> graphVertices;
        std::vector<unsigned>             graphIndices;
        std::vector<glm::vec3>            peakPoints;

        GLAttributeLayout position;
        GLAttributeLayout color;
        GLAttributeLayout uv;

        graphics::Camera camera;
        glm::mat4        projectionMatrix{ 1.0f };

        int   samples;
        float minValue;
        float maxValue;
        float slope;
        float xStart;
        float xEnd;
        float zoomLevel;

        // New members for the moving red circle
        bool  viewValueActive = false;
        bool  viewValuePaused = false;
        float viewValueX      = 0.0f;  // Current x-position of the red circle
        float viewValueSpeed  = 0.05f; // Speed of the red circle movement
    };
}
