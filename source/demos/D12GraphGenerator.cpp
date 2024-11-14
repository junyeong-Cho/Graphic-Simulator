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
    D12GraphGenerator::D12GraphGenerator() : samples(100), minValue(-1.0f), maxValue(1.0f), slope(1.0f), xStart(0.0f), xEnd(10.0f), zoomLevel(1.0f), viewValueSpeed(0.01f), viewValuePaused(true)
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);

        assetReloader.SetAndAutoReloadShader(shader, asset_paths::ShaderName, { asset_paths::CurveVertexPath, asset_paths::CurveFragmentPath });

        glm::vec3 eye_position{ 0, 0, 10 };
        glm::vec3 target_position{ 0, 0, 0 };
        camera = graphics::Camera(eye_position, target_position - eye_position, graphics::Camera::WORLD_UP);

        InitializeCircleMesh();
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

    void D12GraphGenerator::InitializeCircleMesh()
    {
        GLAttributeLayout posAttr;
        GLAttributeLayout colAttr;
        GLAttributeLayout uvAttr;
        graphics::describe_meshvertex_layout(posAttr, colAttr, uvAttr);

        auto circleGeometry = graphics::create_circle(32);
        circleMesh.SetPrimitivePattern(GLPrimitive::Triangles);
        circleMesh.AddVertexBuffer(GLVertexBuffer(std::span{ circleGeometry.Vertices }), { posAttr, colAttr, uvAttr });
        circleMesh.SetIndexBuffer(GLIndexBuffer(std::span{ circleGeometry.Indicies }));
    }

    void D12GraphGenerator::Update()
    {
        assetReloader.Update();
        HandleInput();

        // Move red circle only if viewValueActive and not paused
        if (viewValueActive && !viewValuePaused)
        {
            viewValueX += viewValueSpeed;
            if (viewValueX > xEnd)
            {
                viewValueX = xStart; // Loop back to start if exceeding graph range
            }
        }
    }

    void D12GraphGenerator::HandleInput()
    {
        using namespace environment::input;

        float moveSpeed = 0.05f;
        float zoomSpeed = 0.1f;

        // Camera movement and zoom
        if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::W) != PressedKeyboardButtons.end())
        {
            camera.Move(glm::vec3(0, moveSpeed, 0));
        }
        if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::S) != PressedKeyboardButtons.end())
        {
            camera.Move(glm::vec3(0, -moveSpeed, 0));
        }
        if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::A) != PressedKeyboardButtons.end())
        {
            camera.Move(glm::vec3(-moveSpeed, 0, 0));
        }
        if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::D) != PressedKeyboardButtons.end())
        {
            camera.Move(glm::vec3(moveSpeed, 0, 0));
        }
        if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::Q) != PressedKeyboardButtons.end())
        {
            camera.Move(glm::vec3(0, 0, -zoomSpeed));
        }
        if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::E) != PressedKeyboardButtons.end())
        {
            camera.Move(glm::vec3(0, 0, zoomSpeed));
        }
    }

    void D12GraphGenerator::UpdateGraph()
    {
        graphVertices.clear();
        graphIndices.clear();
        peakPoints.clear();

        float xRange = xEnd - xStart;
        float xStep  = xRange / samples;

        for (int i = 0; i <= samples; ++i)
        {
            float x = xStart + i * xStep;
            float y = slope * sinf(x); // Do not clamp here

            graphVertices.push_back({ glm::vec3(x, y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f) });
            graphIndices.push_back(i);

            // Detect peak points for display purposes if needed
            if (i > 0 && i < samples)
            {
                float prevY = slope * sinf(x - xStep);
                float nextY = slope * sinf(x + xStep);
                if ((y > prevY && y > nextY) || (y < prevY && y < nextY))
                {
                    peakPoints.push_back(glm::vec3(x, y, 0.0f));
                }
            }
        }

        // Set up the graph mesh with updated vertices
        graphMesh = GLVertexArray(GLPrimitive::LineStrip);
        GLAttributeLayout posAttr;
        GLAttributeLayout colAttr;
        GLAttributeLayout uvAttr;
        graphics::describe_meshvertex_layout(posAttr, colAttr, uvAttr);
        graphMesh.AddVertexBuffer(GLVertexBuffer(std::span{ graphVertices }), { posAttr, colAttr, uvAttr });
        graphMesh.SetIndexBuffer(GLIndexBuffer(std::span{ graphIndices }));
    }

    void D12GraphGenerator::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Use();

        glm::mat4 model      = glm::mat4(1.0f);
        glm::mat4 view       = camera.ViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

        glm::mat4 clip = projection * view * model;
        shader.SendUniform("uClipcoords", clip);

        // Draw graph
        graphMesh.Use();
        GLDrawIndexed(graphMesh);

        // Draw circles at peaks
        for (const auto& peak : peakPoints)
        {
            glm::mat4 circleModel = glm::translate(glm::mat4(1.0f), peak) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
            glm::mat4 circleClip  = projection * view * circleModel;
            shader.SendUniform("uClipcoords", circleClip);
            circleMesh.Use();
            GLDrawIndexed(circleMesh);
        }

        // Draw moving red circle if active
        if (viewValueActive)
        {
            float     y = slope * sinf(viewValueX);
            glm::vec3 redCirclePos(viewValueX, y, 0.0f);
            glm::mat4 redCircleModel = glm::translate(glm::mat4(1.0f), redCirclePos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
            glm::mat4 redCircleClip  = projection * view * redCircleModel;
            shader.SendUniform("uClipcoords", redCircleClip);
            circleMesh.Use();
            GLDrawIndexed(circleMesh);
        }
    }

    void D12GraphGenerator::ImGuiDraw()
    {
        ImGui::Text("Sine Graph Generator");

        bool needUpdate = false;

        if (ImGui::SliderFloat("Min Value", &minValue, -10.0f, maxValue - 0.1f))
            needUpdate = true;
        if (ImGui::SliderFloat("Max Value", &maxValue, minValue + 0.1f, 10.0f))
            needUpdate = true;
        if (ImGui::SliderFloat("Slope", &slope, 0.1f, 10.0f))
            needUpdate = true;

        // Display xStart as read-only
        ImGui::Text("X Start: 0.0");
        xStart = 0.0f; // Ensure xStart is fixed to 0

        if (ImGui::SliderFloat("X End", &xEnd, 0.1f, 100.0f))
            needUpdate = true;
        if (ImGui::SliderInt("Samples", &samples, 10, 1000))
            needUpdate = true;

        // Toggle automated movement of the red circle
        if (ImGui::Button(viewValueActive ? "Stop" : "View Value"))
        {
            viewValueActive = !viewValueActive;
            viewValuePaused = false; // Reset pause state
            if (viewValueActive)
                viewValueX = xStart; // Reset position when starting
        }

        // Manual control slider for red circle
        ImGui::SliderFloat("Manual Value Position", &viewValueX, xStart, xEnd);

        // Start/stop and control the speed of the red circle
        if (viewValueActive)
        {
            if (ImGui::Button(viewValuePaused ? "Resume" : "Pause"))
            {
                viewValuePaused = !viewValuePaused;
            }
            ImGui::SliderFloat("Movement Speed", &viewValueSpeed, 0.001f, 0.1f, "%.3f");

            float y = slope * sinf(viewValueX);
            ImGui::Text("Current Value: (%.2f, %.2f)", viewValueX, y);
        }

        ImGui::Text("Graph Equation: y = %.2f * sin(x)", slope);

        if (needUpdate)
            UpdateGraph();
    }
}
