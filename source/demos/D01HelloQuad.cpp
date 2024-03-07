/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D01HelloQuad.hpp"

#include "environment/Environment.hpp"
#include "environment/Input.hpp"
#include "opengl/GL.hpp"
#include <array>
#include <imgui.h>
#include <span>
#include <window/Application.hpp>

namespace
{
    struct float3
    {
        float x = 0, y = 0, z = 0;
    };

    struct color3
    {
        float r = 0, g = 0, b = 0;
    };

    struct texture2
    {
        float s = 0, t = 0;
    };

    struct Vertex
    {
        float3   position{};
        color3   color{};
        texture2 textureCoordinates{};
    };
}

namespace asset_paths
{
    constexpr auto HelloQuadVertexPath   = "D01HelloQuad/01_hello_quad.vert";
    constexpr auto HelloQuadFragmentPath = "D01HelloQuad/01_hello_quad.frag";
    const auto     ShaderName            = "hello quad shader";

    const auto PaintMeTexturePath = "D01HelloQuad/paint_me.png";
}

namespace demos
{
    D01HelloQuad::D01HelloQuad()
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);

        assetReloader.SetAndAutoReloadShader(shader, asset_paths::ShaderName, { asset_paths::HelloQuadVertexPath, asset_paths::HelloQuadFragmentPath });
        assetReloader.SetAndAutoReloadTexture(paintMeTexture, asset_paths::PaintMeTexturePath);

        const std::array vertices = {
            Vertex{float3{ -0.75, 0.75f, 0 }, color3{ 1, 1, 0 }, texture2{ 0.0f, 1.0f }}, // top left
            Vertex{float3{ -0.75, -0.75, 0 }, color3{ 1, 0, 0 }, texture2{ 0.0f, 0.0f }}, // bottom left
            Vertex{ float3{ 0.75, -0.75, 0 }, color3{ 0, 0, 1 }, texture2{ 1.0f, 0.0f }}, // bottom right
            Vertex{  float3{ 0.75, 0.75, 0 }, color3{ 0, 1, 0 }, texture2{ 1.0f, 1.0f }}, // top right
        };
        quadMesh.SetPrimitivePattern(GLPrimitive::TriangleFan);
        const std::array indices       = { 0u, 1u, 2u, 3u };
        const auto       vertices_span = std::span{ vertices };
        GLVertexBuffer   buffer(vertices_span);
        GLIndexBuffer    index_buffer(indices);

        GLAttributeLayout position;
        position.component_type         = GLAttributeLayout::Float;
        position.component_dimension    = GLAttributeLayout::_3;
        position.normalized             = false;
        position.vertex_layout_location = 0;
        position.stride                 = sizeof(Vertex);
        position.offset                 = 0;
        position.relative_offset        = offsetof(Vertex, position);

        GLAttributeLayout color;
        color.component_type         = GLAttributeLayout::Float;
        color.component_dimension    = GLAttributeLayout::_3;
        color.normalized             = false;
        color.vertex_layout_location = 1;
        color.stride                 = sizeof(Vertex);
        color.offset                 = 0;
        color.relative_offset        = offsetof(Vertex, color);

        GLAttributeLayout texture_coordinates;
        texture_coordinates.component_type         = GLAttributeLayout::Float;
        texture_coordinates.component_dimension    = GLAttributeLayout::_2;
        texture_coordinates.normalized             = false;
        texture_coordinates.vertex_layout_location = 2;
        texture_coordinates.stride                 = sizeof(Vertex);
        texture_coordinates.offset                 = 0;
        texture_coordinates.relative_offset        = offsetof(Vertex, textureCoordinates);

        quadMesh.AddVertexBuffer(std::move(buffer), { position, color, texture_coordinates });
        quadMesh.SetIndexBuffer(std::move(index_buffer));


        assert(shader.IsValidWithVertexArrayObject(quadMesh.GetHandle()));
    }

    void D01HelloQuad::Update()
    {
        assetReloader.Update();
       
    }

    void D01HelloQuad::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Use();
        paintMeTexture.UseForSlot(0);
        shader.SendUniform("uTex2d", 0);
        shader.SendUniform("uTime", static_cast<float>(environment::ElapsedTime));
        quadMesh.Use();
        GLDrawIndexed(quadMesh);
    }

    void D01HelloQuad::ImGuiDraw()
    {
        ImGui::Text("%s", "Hello Quad!!!");
    }
}
