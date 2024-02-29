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
#include "opengl/GLShader.hpp"
#include "opengl/GLTexture.hpp"
#include "opengl/GLVertexArray.hpp"

namespace demos
{
    class D01HelloQuad : public IDemo
    {
    public:
        D01HelloQuad();

        void Update() override;

        void Draw() const override;
        void ImGuiDraw() override;

        constexpr void SetDisplaySize([[maybe_unused]] int width, [[maybe_unused]] int height) override
        {
        }

    private:
        GLShader         shader;
        GLVertexArray    quadMesh;
        GLTexture        paintMeTexture;
        assets::Reloader assetReloader;
    };

}
