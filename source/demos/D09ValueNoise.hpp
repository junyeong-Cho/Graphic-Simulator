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
#include "graphics/noise/ValueNoise.hpp"
#include "opengl/GLShader.hpp"
#include "opengl/GLTexture.hpp"
#include <vector>

#include "util/JobSystem.hpp"
#include "util/Timer.hpp"

namespace demos
{
    class D09ValueNoise : public IDemo
    {
    public:
        D09ValueNoise();

        void Update() override;

        void Draw() const override;
        void ImGuiDraw() override;

        void SetDisplaySize(int width, int height) override;

    private:
        assets::Reloader                       assetReloader;
        glm::mat4                              orthoProjectionMatrix{};
        GLShader                               displayTextureShader;
        graphics::Material                     displayTextureMaterial;
        graphics::SubMesh                      quadMesh;
        int                                    textureSize = 2048;
        GLTexture                              generatedTexture;
        graphics::noise::ValueNoise<glm::vec4> noise{ graphics::noise::PeriodDimension::_64 };
        std::vector<GLTexture::RGBA>           colors;
        graphics::noise::PeriodDimension       noisePeriod       = graphics::noise::PeriodDimension::_64;
        graphics::noise::SmoothMethod          noiseSmoothMethod = graphics::noise::SmoothMethod::Quintic;
        glm::vec2                              tileScale{ 0.5f };
        glm::vec2                              targetTileScale{ 1.0f };
        float                                  targetZ = 0;

        struct ValueSet
        {
            enum Type
            {
                RandomGrey,
                RandomColor,
                UniformGrey
            };
        };

        ValueSet::Type lastPickedValueSet = ValueSet::RandomGrey;

        struct Dimension
        {
            enum Type
            {
                _1D,
                _2D,
                _3D
            };
        };

        struct Pattern
        {
            enum Type
            {
                PlainValue,
                FractalSum,
                Turbulence,
                Marble,
                Wood,
            };
        };

        struct Generation
        {
            enum State
            {
                Setup,
                Working,
                UploadNewTexture,
                Done
            } state = Done;

            std::vector<float> xyInputValues;
            float              frequency      = 1;
            int                column         = 0;
            int                row            = 0;
            int                width          = 0;
            int                height         = 0;
            float              z              = 0;
            float              y              = 0;
            Dimension::Type    noiseDimension = Dimension::_2D;
            Pattern::Type      pattern        = Pattern::PlainValue;
            GLTexture::RGBA*   the_colors     = nullptr;
            util::Timer        timer;
            util::JobSystem    jobSystem;


            void            setup(D09ValueNoise& demo);
            void            update(const D09ValueNoise& demo);
            GLTexture::RGBA get_color(const graphics::noise::ValueNoise<glm::vec4>& the_noise, float the_x, float the_y, float the_z) const;
        } generation;
    };


}
