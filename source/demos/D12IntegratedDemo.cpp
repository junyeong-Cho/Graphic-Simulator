/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D12IntegratedDemo.hpp"

#include "D01HelloQuad.hpp"
#include "D02ProceduralMeshes.hpp"
#include "D03Fog.hpp"
#include "D04ToonShading.hpp"
#include "D05ShadowMapping.hpp"
#include "D06GeometryShaders.hpp"
#include "D07TessellationShaders.hpp"
#include "D08ComputeShaders.hpp"
#include "D09ValueNoise.hpp"
#include "D10GradientNoise.hpp"
#include "D11CurvesNSplines.hpp"

#include "environment/Environment.hpp"
#include "environment/Input.hpp"
#include "environment/OpenGL.hpp"
#include "graphics/MathHelper.hpp"
#include "opengl/GL.hpp"
#include <SDL.h>
#include <glm/ext/matrix_clip_space.hpp> // perspective
#include <imgui.h>
#include <span>

namespace
{
    enum class DemoType
    {
        BasicRendering,
        Transformations,
        Lighting,
        ToonShading,
        ShadowMapping,
        GeometryShaders,
        TessellationShaders,
        ComputeShaders,
        ValueNoise,
        GradientNoise,
        CurvesNSplines,
        Count
    };

    DemoType currentDemo = DemoType::BasicRendering;

    demos::D01HelloQuad           basicRenderingDemo;
    demos::D02ProceduralMeshes    transformationsDemo;
    demos::D03Fog                 lightingDemo;
    demos::D04ToonShading         toonShadingDemo;
    demos::D05ShadowMapping       shadowMappingDemo;
    demos::D06GeometryShaders     geometryShadersDemo;
    demos::D07TessellationShaders tessellationShadersDemo;
    demos::D08ComputeShaders      computeShadersDemo;
    demos::D09ValueNoise          valueNoiseDemo;
    demos::D10GradientNoise       gradientNoiseDemo;
    demos::D11CurvesNSplines      curvesNSplinesDemo;
}

namespace demos
{
    D12IntegratedDemo::D12IntegratedDemo()
    {
        SetDisplaySize(environment::DisplayWidth, environment::DisplayHeight);
    }

    void D12IntegratedDemo::Update()
    {
        switch (currentDemo)
        {
            case DemoType::BasicRendering: basicRenderingDemo.Update(); break;
            case DemoType::Transformations: transformationsDemo.Update(); break;
            case DemoType::Lighting: lightingDemo.Update(); break;
            case DemoType::ToonShading: toonShadingDemo.Update(); break;
            case DemoType::ShadowMapping: shadowMappingDemo.Update(); break;
            case DemoType::GeometryShaders: geometryShadersDemo.Update(); break;
            case DemoType::TessellationShaders: tessellationShadersDemo.Update(); break;
            case DemoType::ComputeShaders: computeShadersDemo.Update(); break;
            case DemoType::ValueNoise: valueNoiseDemo.Update(); break;
            case DemoType::GradientNoise: gradientNoiseDemo.Update(); break;
            case DemoType::CurvesNSplines: curvesNSplinesDemo.Update(); break;
            default: break;
        }
    }

    void D12IntegratedDemo::Draw() const
    {
        switch (currentDemo)
        {
            case DemoType::BasicRendering: basicRenderingDemo.Draw(); break;
            case DemoType::Transformations: transformationsDemo.Draw(); break;
            case DemoType::Lighting: lightingDemo.Draw(); break;
            case DemoType::ToonShading: toonShadingDemo.Draw(); break;
            case DemoType::ShadowMapping: shadowMappingDemo.Draw(); break;
            case DemoType::GeometryShaders: geometryShadersDemo.Draw(); break;
            case DemoType::TessellationShaders: tessellationShadersDemo.Draw(); break;
            case DemoType::ComputeShaders: computeShadersDemo.Draw(); break;
            case DemoType::ValueNoise: valueNoiseDemo.Draw(); break;
            case DemoType::GradientNoise: gradientNoiseDemo.Draw(); break;
            case DemoType::CurvesNSplines: curvesNSplinesDemo.Draw(); break;
            default: break;
        }
    }

    void D12IntegratedDemo::ImGuiDraw()
    {
        const char* demoNames[] = { "Basic Rendering",      "Transformations", "Lighting",    "Toon Shading",   "Shadow Mapping",    "Geometry Shaders",
                                    "Tessellation Shaders", "Compute Shaders", "Value Noise", "Gradient Noise", "Curves and Splines" };

        if (ImGui::BeginCombo("Select Demo", demoNames[static_cast<int>(currentDemo)]))
        {
            for (int n = 0; n < static_cast<int>(DemoType::Count); ++n)
            {
                bool isSelected = (currentDemo == static_cast<DemoType>(n));
                if (ImGui::Selectable(demoNames[n], isSelected))
                {
                    currentDemo = static_cast<DemoType>(n);
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        switch (currentDemo)
        {
            case DemoType::BasicRendering: basicRenderingDemo.ImGuiDraw(); break;
            case DemoType::Transformations: transformationsDemo.ImGuiDraw(); break;
            case DemoType::Lighting: lightingDemo.ImGuiDraw(); break;
            case DemoType::ToonShading: toonShadingDemo.ImGuiDraw(); break;
            case DemoType::ShadowMapping: shadowMappingDemo.ImGuiDraw(); break;
            case DemoType::GeometryShaders: geometryShadersDemo.ImGuiDraw(); break;
            case DemoType::TessellationShaders: tessellationShadersDemo.ImGuiDraw(); break;
            case DemoType::ComputeShaders: computeShadersDemo.ImGuiDraw(); break;
            case DemoType::ValueNoise: valueNoiseDemo.ImGuiDraw(); break;
            case DemoType::GradientNoise: gradientNoiseDemo.ImGuiDraw(); break;
            case DemoType::CurvesNSplines: curvesNSplinesDemo.ImGuiDraw(); break;
            default: break;
        }
    }

    void D12IntegratedDemo::SetDisplaySize(int width, int height)
    {
        basicRenderingDemo.SetDisplaySize(width, height);
        transformationsDemo.SetDisplaySize(width, height);
        lightingDemo.SetDisplaySize(width, height);
        toonShadingDemo.SetDisplaySize(width, height);
        shadowMappingDemo.SetDisplaySize(width, height);
        geometryShadersDemo.SetDisplaySize(width, height);
        tessellationShadersDemo.SetDisplaySize(width, height);
        computeShadersDemo.SetDisplaySize(width, height);
        valueNoiseDemo.SetDisplaySize(width, height);
        gradientNoiseDemo.SetDisplaySize(width, height);
        curvesNSplinesDemo.SetDisplaySize(width, height);
    }
}