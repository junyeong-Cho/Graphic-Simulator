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

namespace demos
{
    class D03Fog : public IDemo
    {
    public:
        D03Fog();

        void Update() override;

        void Draw() const override;
        void ImGuiDraw() override;

        void SetDisplaySize(int width, int height) override;

    private:

        struct ObjectModel
        {
            enum Type
            {
                Cube,
                Sphere,
                Count

            };
        };

        struct Materials
        {
            enum Type
            {
                Crate,
                PoolBall,
                Count
            };
        };

        struct FogStyle
        {
            enum Type
            {
                Linear,
                Exponential,
                FragCoordZ,
                Count
            };
        };

        struct SceneObject
        {
            glm::vec3                 Translation{ 1.0f };
            glm::vec3                 EulerAngles{};
            ObjectModel::Type         Model = ObjectModel::Sphere;
            std::function<void(void)> Update;
        };

        assets::Reloader                                 assetReloader;
        std::array<GLShader, FogStyle::Count>            shaders;
        std::array<graphics::Mesh, ObjectModel::Count>   meshesTriangles;
        std::array<graphics::Material, Materials::Count> materials;
        GLTexture                                        textures[Materials::Count];
        std::vector<SceneObject>                         sceneObjects;
        glm::mat4                                        ProjectionMatrix;
        glm::mat4                                        ViewMatrix;

        glm::vec2 modelRotations{};

        constexpr static glm::vec3 FogColor{ 0.9f, 0.98f, 0.99f };

        FogStyle::Type fogStyle = FogStyle::Linear;

        constexpr static glm::vec2 LinearFogMaxRange{ 0.0f, 40.0f };
        float                      fogNearDistance       = LinearFogMaxRange.x;
        float                      targetFogNearDistance = 1.1f;
        float                      fogFarDistance        = LinearFogMaxRange.y;
        float                      targetFogFarDistance  = 13.0f;

        constexpr static glm::vec2 ExponentialFogMaxRange{ 0.0f, 1.0f };
        float                      fogDensity       = ExponentialFogMaxRange.x;
        float                      targetFogDensity = 0.092f;

        constexpr static glm::vec2 FragCoordZFogMaxRange{ 0.0f, 1.0f };
        float                      fragCoordZFogNearDistance      = FragCoordZFogMaxRange.x;
        float                      targetFragCoordZFNearDistance  = 0.7f;
        float                      fragCoordZFFarDistance         = FragCoordZFogMaxRange.y;
        float                      targetFragCoordZFogFarDistance = 0.9f;

    private:
        void setMaterialsForShader(FogStyle::Type shaded_type);
        void buildMeshes();
        void createLongLineSceneObjects();
        void createCirclingSceneObjects();
    };

}
