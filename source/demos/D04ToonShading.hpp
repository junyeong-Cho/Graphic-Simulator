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
    class D04ToonShading : public IDemo
    {
    public:

        D04ToonShading();

        void Update() override;

        void Draw() const override;
        void ImGuiDraw() override;

        void SetDisplaySize(int width, int height) override;

    private:
        struct ObjectModel
        {
            enum Type
            {
                Trefoil,
                Plane,
                Cube,
                Sphere,
                Torus,
                Cylinder,
                Cone,
                Count

            };
        };

        struct ToonStyle
        {
            enum Type
            {
                CustomRange,
                UniformRange,
                GradientTexture,
                Count,
                All = Count
            };
        };

        struct Gradients
        {
            enum Type
            {
                Colored01,
                Colored02,
                FiveTone,
                ThreeTone,
                Count
            };
        };

        using MeshVariations   = std::array<graphics::Mesh, ToonStyle::Count>;
        using MeshCombinations = std::array<MeshVariations, ObjectModel::Count>;

        Gradients::Type                                  gradientType = Gradients::Colored01;
        std::array<GLShader, ToonStyle::Count>           shaders;
        std::array<GLTexture, Gradients::Count>          gradientTextures;
        std::array<graphics::Material, ToonStyle::Count> materials;
        MeshCombinations                                 meshes;
        assets::Reloader                                 assetReloader;
        glm::mat4                                        ProjectionMatrix;
        glm::mat4                                        ViewMatrix;
        glm::vec3                                        eyePosition{ 0.0f };
        glm::vec3                                        targetEyePosition{ 0.0f, -0.05f, 0.0f };
        ObjectModel::Type                                selectedObjectModel = ObjectModel::Trefoil;
        ObjectModel::Type                                previousObjectModel = ObjectModel::Plane;
        ToonStyle::Type                                  focus               = ToonStyle::All;
        bool                                             autoRotate          = true;
        bool                                             enableAntialising   = true;
        float                                            rotationAngle       = 0;
        constexpr static glm::vec3                       FogColor{ 0.9f, 0.98f, 0.99f };
        float                                            fogNearDistance = 1.0f;
        float                                            fogFarDistance  = 5.0f;
        glm::vec3                                        diffuse{ 0, 0.75, 0.75 };
        glm::vec3                                        ambient{ 0.04f };
        glm::vec3                                        specular{ 0.5f };
        float                                            shininess = 50;
        glm::vec3                                        lightDirection{ 0.0f, 1.0f, 0.0f };
        glm::vec3                                        targetLightDirection{ 0.25f, 0.25f, 1.0f };

        struct
        {
            float level1 = 0.1f;
            float level2 = 0.3f;
            float level3 = 0.6f;
            float max    = 1.0f;
        } customRangeSettings;

        struct
        {
            int diffuseChunks = 5;
        } uniformRangeSettings;

        static constexpr float XOffset         = 0.75f;
        constexpr static float ViewAllDistance = 1.5f;
        constexpr static float ViewOneDistance = 0.8f;

        struct
        {
            glm::vec3 center;
            float     scale = 0.65f;
        } camPresets[ObjectModel::Count];

    private:
        void setLookAt();
        void buildMeshes();
        void drawObject(const glm::mat4& t, const glm::mat4& r, ObjectModel::Type model, ToonStyle::Type mesh_type) const;
    };

}
