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
#include "graphics/Mesh.hpp"
#include "opengl/GLShader.hpp"
#include "opengl/GLTexture.hpp"
#include <array>

namespace demos
{
    class D08ComputeShaders : public IDemo
    {
    public:
        D08ComputeShaders();

        void Update() override;

        void Draw() const override;
        void ImGuiDraw() override;

        void SetDisplaySize(int width, int height) override;

    private:

        struct Example
        {
            enum Type
            {
                Simple,
                Luminance,
                CircleColorPulse,
                Fun,
                Count
            };
        };

        std::array<GLShader, Example::Count> computeShaders;
        GLShader                             displayTextureShader;
        graphics::SubMesh                    quadMesh;
        graphics::Material                   displayTextureMaterial;
        GLTexture                            generatedTexture;
        GLTexture                            bellTexture;
        assets::Reloader                     assetReloader;
        glm::mat4                            orthoProjectionMatrix;
        Example::Type                        selectedComputeShader = Example::CircleColorPulse;
        int                                  textureSize           = 256;
        std::array<GLuint, 3>                dispatchSize;
        std::array<GLint, 3>                 localWorkgroupSize;

    private:
        void updateDispatchSize();
    };

}
