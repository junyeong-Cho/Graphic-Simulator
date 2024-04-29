/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "IDemo.hpp"
#include "assets/Reloader.hpp"
#include "graphics/Material.hpp"
#include "opengl/GLShader.hpp"
#include "opengl/GLTexture.hpp"
#include "opengl/GLVertexArray.hpp"

namespace demos
{
    class D07TessellationShaders : public IDemo
    {
    public:
        D07TessellationShaders();

        void Update() override;

        void Draw() const override;
        void ImGuiDraw() override;

        void SetDisplaySize([[maybe_unused]] int width, [[maybe_unused]] int height) override;

    private:
        struct Materials
        {
            enum Type : unsigned
            {
                QuadEqualSpacing,
                QuadEvenSpacing,
                QuadOddSpacing,
                TriEqualSpacing,
                TriEvenSpacing,
                TriOddSpacing,
                LineEqualSpacing,
                LineEvenSpacing,
                LineOddSpacing,
                Count
            };
        };

        struct Tessellation
        {
            enum Spacing : int
            {
                equal_spacing,
                fractional_even_spacing,
                fractional_odd_spacing,
                SpacingCount
            };

            enum Shape : int
            {
                quads,
                triangles,
                isolines,
                ShapeCount
            };
        };

        std::array<graphics::Material, Materials::Count>    materials;
        std::array<GLShader, Materials::Count>              shaders;
        std::array<GLVertexArray, Tessellation::ShapeCount> meshes;
        assets::Reloader                                    assetReloader;
        glm::mat4                                           ProjectionMatrix{ 1.0f };
        glm::mat4                                           ViewMatrix{ 1.0f };
        std::array<glm::vec4, Tessellation::ShapeCount>     outsideTessellation{ glm::vec4{ 1.0f }, glm::vec4{ 1.0f }, glm::vec4{ 1.0f } };
        std::array<glm::vec2, Tessellation::ShapeCount>     insideTessellation{ glm::vec2{ 1.0f }, glm::vec2{ 1.0f }, glm::vec2{ 1.0f } };
        glm::vec4                                           targetOutsideTessellation{ 1.0f };
        glm::vec2                                           targetInsideTessellation{ 1.0f };
        Tessellation::Spacing                               spacing{};
        Tessellation::Shape                                 shape{};

    private:
        void buildMeshes();
    };

}
