/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "D12GraphGenerator.hpp"

#include "environment/Environment.hpp"
#include "environment/Input.hpp"
#include "environment/OpenGL.hpp"

#include "graphics/MathHelper.hpp"
#include "graphics/Mesh.hpp"

#include "opengl/GL.hpp"

#include <SDL.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <span>

namespace asset_paths
{
    constexpr auto CurveVertexPath   = "D12GraphGenerator/curve.vert";
    constexpr auto CurveFragmentPath = "D12GraphGenerator/curve.frag";
    const auto     ShaderName        = "curve shader";
}

namespace demos
{
    D12GraphGenerator::D12GraphGenerator() : samples(100), minValue(-1.0f), maxValue(1.0f), slope(1.0f), xStart(-10.0f), xEnd(10.0f), zoomLevel(1.0f)
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);

        assetReloader.SetAndAutoReloadShader(shader, asset_paths::ShaderName, { asset_paths::CurveVertexPath, asset_paths::CurveFragmentPath });

        UpdateGraph();

        GLAttributeLayout posAttr;
        GLAttributeLayout colAttr;
        GLAttributeLayout uvAttr;
        graphics::describe_meshvertex_layout(posAttr, colAttr, uvAttr);

        graphMesh.SetPrimitivePattern(GLPrimitive::LineStrip);
        graphMesh.AddVertexBuffer(GLVertexBuffer(std::span{ graphVertices }), { posAttr, colAttr, uvAttr });
        graphMesh.SetIndexBuffer(GLIndexBuffer(std::span{ graphIndices }));

        assert(shader.IsValidWithVertexArrayObject(graphMesh.GetHandle()));
    }

    void D12GraphGenerator::Update()
    {
        assetReloader.Update();
        HandleInput();
    }

    void D12GraphGenerator::HandleInput()
    {
        // Handle any necessary inputs
    }

    void D12GraphGenerator::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view  = glm::mat4(1.0f);

        float zoomFactor = 1.0f / zoomLevel;

        // Calculate centers and ranges
        float xCenter = (xStart + xEnd) / 2.0f;
        float yCenter = (minValue + maxValue) / 2.0f;

        float xRange = (xEnd - xStart) * zoomFactor;
        float yRange = (maxValue - minValue) * zoomFactor;

        float left   = xCenter - xRange / 2.0f;
        float right  = xCenter + xRange / 2.0f;
        float bottom = yCenter - yRange / 2.0f;
        float top    = yCenter + yRange / 2.0f;

        glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        glm::mat4 clip       = projection * view * model;

        shader.SendUniform("uClipcoords", clip);

        graphMesh.Use();
        GLDrawIndexed(graphMesh);
    }

    void D12GraphGenerator::ImGuiDraw()
    {
        ImGui::Text("%s\n\n", "Sine Graph Generator");

        bool needUpdate = false;

        if (ImGui::SliderFloat("Min Value", &minValue, -10.0f, maxValue - 0.1f))
        {
            needUpdate = true;
        }
        if (ImGui::SliderFloat("Max Value", &maxValue, minValue + 0.1f, 10.0f))
        {
            needUpdate = true;
        }
        if (ImGui::SliderFloat("Slope", &slope, 0.1f, 10.0f))
        {
            needUpdate = true;
        }
        if (ImGui::SliderFloat("X Start", &xStart, -100.0f, xEnd - 0.1f))
        {
            needUpdate = true;
        }
        if (ImGui::SliderFloat("X End", &xEnd, xStart + 0.1f, 100.0f))
        {
            needUpdate = true;
        }
        if (ImGui::SliderInt("Samples", &samples, 10, 1000))
        {
            needUpdate = true;
        }
        if (ImGui::SliderFloat("Zoom", &zoomLevel, 0.1f, 10.0f, "%.1f"))
        {
            // Zoom level affects the projection, so no need to update the graph data
        }

        if (needUpdate)
        {
            UpdateGraph();
        }
    }

    void D12GraphGenerator::UpdateGraph()
    {
        graphVertices.clear();
        graphIndices.clear();

        float xRange = xEnd - xStart;
        float xStep  = xRange / samples;

        for (int i = 0; i <= samples; ++i)
        {
            float x = xStart + i * xStep;
            float y = slope * sinf(x);

            // Clamp y to minValue and maxValue
            y = glm::clamp(y, minValue, maxValue);

            graphVertices.push_back({ glm::vec3(x, y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f) });
            graphIndices.push_back(i);
        }

        // Re-initialize the vertex array object instead of clearing it
        graphMesh = GLVertexArray(GLPrimitive::LineStrip);

        GLAttributeLayout posAttr;
        GLAttributeLayout colAttr;
        GLAttributeLayout uvAttr;
        graphics::describe_meshvertex_layout(posAttr, colAttr, uvAttr);

        graphMesh.AddVertexBuffer(GLVertexBuffer(std::span{ graphVertices }), { posAttr, colAttr, uvAttr });
        graphMesh.SetIndexBuffer(GLIndexBuffer(std::span{ graphIndices }));

        assert(shader.IsValidWithVertexArrayObject(graphMesh.GetHandle()));
    }
}
