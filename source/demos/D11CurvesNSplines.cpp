/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "D11CurvesNSplines.hpp"
#include "graphics/curve/CurveGeneration.hpp"

#include "environment/Environment.hpp"
#include "environment/Input.hpp"
#include "environment/OpenGL.hpp"

#include "graphics/MathHelper.hpp"
#include "graphics/Mesh.hpp"

#include "opengl/GL.hpp"

#include <SDL.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/curve/CurveGeneration.hpp>
#include <imgui.h>
#include <span>

namespace asset_paths
{
    constexpr auto CurveVertexPath   = "D11CurvesNSplines/curve.vert";
    constexpr auto CurveFragmentPath = "D11CurvesNSplines/curve.frag";
    const auto     ShaderName        = "curve shader";
}

namespace demos
{
    D11CurvesNSplines::D11CurvesNSplines() : selectedPointIndex(-1), selectedCurveType(CurveType::All), samples(40), tangentLength(0.1f), zoomLevel(45.0f)
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);

        assetReloader.SetAndAutoReloadShader(shader, asset_paths::ShaderName, { asset_paths::CurveVertexPath, asset_paths::CurveFragmentPath });

        initialControlPoints = { glm::vec3(-0.75f, 0.0f, 0.0f), glm::vec3(-0.25f, 0.75f, 0.0f), glm::vec3(0.25f, -0.75f, 0.0f), glm::vec3(0.75f, 0.0f, 0.0f) };
        initialTangents      = { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f) };

        controlPoints = initialControlPoints;
        tangents      = initialTangents;

        UpdateCurves();

        GLAttributeLayout posAttr;
        GLAttributeLayout colAttr;
        GLAttributeLayout uvAttr;
        graphics::describe_meshvertex_layout(posAttr, colAttr, uvAttr);

        hermiteMesh.SetPrimitivePattern(GLPrimitive::Lines);
        hermiteMesh.AddVertexBuffer(GLVertexBuffer(std::span{ hermiteVertices }), { posAttr, colAttr, uvAttr });
        hermiteMesh.SetIndexBuffer(GLIndexBuffer(std::span{ hermiteIndices }));

        catmullMesh.SetPrimitivePattern(GLPrimitive::Lines);
        catmullMesh.AddVertexBuffer(GLVertexBuffer(std::span{ catmullVertices }), { posAttr, colAttr, uvAttr });
        catmullMesh.SetIndexBuffer(GLIndexBuffer(std::span{ catmullIndices }));

        tangentMesh.SetPrimitivePattern(GLPrimitive::Lines);
        tangentMesh.AddVertexBuffer(GLVertexBuffer(std::span{ tangentVertices }), { posAttr, colAttr, uvAttr });
        tangentMesh.SetIndexBuffer(GLIndexBuffer(std::span{ tangentIndices }));

        assert(shader.IsValidWithVertexArrayObject(hermiteMesh.GetHandle()));
        assert(shader.IsValidWithVertexArrayObject(catmullMesh.GetHandle()));
        assert(shader.IsValidWithVertexArrayObject(tangentMesh.GetHandle()));

        auto circleGeometry = graphics::create_circle(32);
        circleMesh.SetPrimitivePattern(GLPrimitive::Triangles);
        circleMesh.AddVertexBuffer(GLVertexBuffer(std::span{ circleGeometry.Vertices }), { posAttr, colAttr, uvAttr });
        circleMesh.SetIndexBuffer(GLIndexBuffer(std::span{ circleGeometry.Indicies }));
    }

    void D11CurvesNSplines::Update()
    {
        assetReloader.Update();
        HandleInput();
    }

    void D11CurvesNSplines::HandleInput()
    {
        using namespace environment::input;

        float moveSpeed = 0.01f;

        if (selectedPointIndex != -1)
        {
            if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::W) != PressedKeyboardButtons.end())
            {
                controlPoints[selectedPointIndex].y += moveSpeed;
            }
            if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::S) != PressedKeyboardButtons.end())
            {
                controlPoints[selectedPointIndex].y -= moveSpeed;
            }
            if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::A) != PressedKeyboardButtons.end())
            {
                controlPoints[selectedPointIndex].x -= moveSpeed;
            }
            if (std::find(PressedKeyboardButtons.begin(), PressedKeyboardButtons.end(), KeyboardButtons::D) != PressedKeyboardButtons.end())
            {
                controlPoints[selectedPointIndex].x += moveSpeed;
            }
        }

        UpdateCurves();
    }

    void D11CurvesNSplines::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Use();

        glm::mat4 model      = glm::mat4(1.0f);
        glm::mat4 view       = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
        glm::mat4 projection = glm::perspective(glm::radians(zoomLevel), 1.0f, 0.1f, 10.0f);
        glm::mat4 clip       = projection * view * model;

        shader.SendUniform("uClipcoords", clip);

        if (selectedCurveType == CurveType::Hermite || selectedCurveType == CurveType::All)
        {
            hermiteMesh.Use();
            GLDrawIndexed(hermiteMesh);
        }

        if (selectedCurveType == CurveType::Catmull || selectedCurveType == CurveType::All)
        {
            catmullMesh.Use();
            GLDrawIndexed(catmullMesh);
        }

        if (selectedCurveType == CurveType::All)
        {
            tangentMesh.Use();
            GLDrawIndexed(tangentMesh);
        }

        for (const auto& point : controlPoints)
        {
            glm::mat4 circleModel = glm::translate(glm::mat4(1.0f), point) * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
            glm::mat4 circleClip  = projection * view * circleModel;
            shader.SendUniform("uClipcoords", circleClip);
            circleMesh.Use();
            GLDrawIndexed(circleMesh);
        }
    }

    void D11CurvesNSplines::ImGuiDraw()
    {
        ImGui::Text("%s\n\n", "Hermite Curve and Catmull-Rom Spline with Tangents");

        constexpr const char* items[] = { "Hermite", "Catmull", "All" };
        constexpr CurveType   types[] = { CurveType::Hermite, CurveType::Catmull, CurveType::All };

        auto              current_focus       = selectedCurveType;
        const char* const combo_preview_value = items[static_cast<int>(current_focus)];
        if (ImGui::BeginCombo("Select Curve Type", combo_preview_value, 0))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (current_focus == types[n]);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    current_focus = types[n];
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (current_focus != selectedCurveType)
        {
            selectedCurveType = current_focus;
        }

        ImGui::Text("\n%s", "WASD to move selected control point");
        for (size_t i = 0; i < controlPoints.size(); ++i)
        {
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::RadioButton(("Control Point " + std::to_string(i)).c_str(), selectedPointIndex == static_cast<int>(i)))
            {
                selectedPointIndex = static_cast<int>(i);
            }
            ImGui::PopID();
        }

        if (ImGui::Button("Add Control Point"))
        {
            controlPoints.emplace_back(0.0f, 0.0f, 0.0f);
            tangents.emplace_back(1.0f, 0.0f, 0.0f);

            UpdateCurves();
        }

        if (ImGui::Button("Reset Points"))
        {
            controlPoints      = initialControlPoints;
            tangents           = initialTangents;
            selectedPointIndex = -1;
            UpdateCurves();
        }

        ImGui::SliderInt("Samples", &samples, 0, 100);

        // Add zoom slider
        ImGui::SliderFloat("Zoom", &zoomLevel, 1.0f, 90.0f, "%.1f");

        UpdateCurves();
    }

    void D11CurvesNSplines::UpdateCurves()
    {
        auto hermiteCurve     = graphics::generateHermiteCurve(controlPoints, tangents, samples);
        auto catmullRomSpline = graphics::generateCatmullRomSpline(controlPoints, samples);

        hermiteVertices.clear();
        hermiteIndices.clear();
        for (unsigned i = 0; i < hermiteCurve.size(); ++i)
        {
            hermiteVertices.push_back({ hermiteCurve[i], glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f) });
            if (i > 0)
            {
                hermiteIndices.push_back(i - 1);
                hermiteIndices.push_back(i);
            }
        }

        catmullVertices.clear();
        catmullIndices.clear();
        for (unsigned i = 0; i < catmullRomSpline.size(); ++i)
        {
            catmullVertices.push_back({ catmullRomSpline[i], glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f) });
            if (i > 0)
            {
                catmullIndices.push_back(i - 1);
                catmullIndices.push_back(i);
            }
        }

        tangentVertices.clear();
        tangentIndices.clear();
        for (size_t i = 0; i < controlPoints.size(); ++i)
        {
            tangentVertices.push_back({ controlPoints[i], glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f) });
            tangentVertices.push_back({ controlPoints[i] + tangents[i] * tangentLength, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f) });

            tangentIndices.push_back(static_cast<unsigned>(i * 2));
            tangentIndices.push_back(static_cast<unsigned>(i * 2 + 1));
        }

        GLAttributeLayout posAttr;
        GLAttributeLayout colAttr;
        GLAttributeLayout uvAttr;
        graphics::describe_meshvertex_layout(posAttr, colAttr, uvAttr);

        hermiteMesh = GLVertexArray(GLPrimitive::Lines);
        hermiteMesh.AddVertexBuffer(GLVertexBuffer(std::span{ hermiteVertices }), { posAttr, colAttr, uvAttr });
        hermiteMesh.SetIndexBuffer(GLIndexBuffer(std::span{ hermiteIndices }));

        catmullMesh = GLVertexArray(GLPrimitive::Lines);
        catmullMesh.AddVertexBuffer(GLVertexBuffer(std::span{ catmullVertices }), { posAttr, colAttr, uvAttr });
        catmullMesh.SetIndexBuffer(GLIndexBuffer(std::span{ catmullIndices }));

        tangentMesh = GLVertexArray(GLPrimitive::Lines);
        tangentMesh.AddVertexBuffer(GLVertexBuffer(std::span{ tangentVertices }), { posAttr, colAttr, uvAttr });
        tangentMesh.SetIndexBuffer(GLIndexBuffer(std::span{ tangentIndices }));

        assert(shader.IsValidWithVertexArrayObject(hermiteMesh.GetHandle()));
        assert(shader.IsValidWithVertexArrayObject(catmullMesh.GetHandle()));
        assert(shader.IsValidWithVertexArrayObject(tangentMesh.GetHandle()));
    }
}
