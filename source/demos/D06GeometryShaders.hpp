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
    class D06GeometryShaders : public IDemo
    {
    public:

        D06GeometryShaders();

        void Update() override;

        void Draw() const override;
        void ImGuiDraw() override;

        void SetDisplaySize(int width, int height) override;


    private:
        struct ObjectModel
        {
            enum Type
            {
                Plane,
                Cube,
                Sphere,
                Torus,
                Cylinder,
                Cone,
                Count

            };
        };

        struct SceneObject
        {
            glm::vec3         Translation{ 0.0f };
            ObjectModel::Type Model = ObjectModel::Sphere;
        };

        struct Materials
        {
            enum Type : unsigned
            {
                Textured,
                Wireframe,
                Normals,
                Extrude,
                Shrink,
                Fun,
                Count
            };
        };

        assets::Reloader                                         assetReloader;
        Materials::Type                                          currentMaterial = Materials::Textured;
        GLTexture                                                uvTexture;
        std::array<graphics::Mesh, ObjectModel::Count>           meshes;
        mutable std::array<graphics::Material, Materials::Count> materials;
        std::array<GLShader, Materials::Count>                   shaders;
        std::vector<SceneObject>                                 sceneObjects;
        glm::mat4                                                ProjectionMatrix{ 1.0f };
        glm::mat4                                                ViewMatrix{ 1.0f };
        glm::vec3                                                eyePosition{ 0.0f };
        glm::vec3                                                targetEyePosition{ 0.0f };
        constexpr static float                                   ViewAllDistance    = 1.5f;
        constexpr static float                                   ViewOneDistance    = 0.6f;
        float                                                    viewDistance       = ViewOneDistance;
        float                                                    targetViewDistance = ViewAllDistance;

        ObjectModel::Type selectedObjectModel = ObjectModel::Count;
        int               stacks              = 20;
        int               slices              = 20;
        bool              autoRotate          = true;
        float             rotationAngle       = 0;
        float             extrudeFactor       = 0.01f;
        bool              extrudeFlatly       = true;
        float             shrinkFactor        = 0.9f;

    private:
        void setViewMatrix(glm::vec3 target_position, float distance = 1.5f);
        void drawSceneObjects(const glm::mat4& r, graphics::Material& material) const;
        void buildMeshes();
    };

}
