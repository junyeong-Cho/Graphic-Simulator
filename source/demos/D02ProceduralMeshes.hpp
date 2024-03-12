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
    class D02ProceduralMeshes : public IDemo
    {
    public:

        D02ProceduralMeshes();

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
            glm::vec3         Translation;
            ObjectModel::Type Model = ObjectModel::Sphere;
        };

        struct Materials
        {
            enum Type
            {
                Textured,
                TexturedPlane,
                Wireframe,
                Normals,
                Count
            };
        };

        GLShader                                         fillShader;
        GLShader                                         texturedShader;
        assets::Reloader                                 assetReloader;
        GLTexture                                        uvTexture;
        std::array<graphics::Mesh, ObjectModel::Count>   meshesTriangles;
        std::array<graphics::Mesh, ObjectModel::Count>   meshesLines;
        std::array<graphics::Mesh, ObjectModel::Count>   meshesNormals;
        std::array<graphics::Material, Materials::Count> materials;
        std::vector<SceneObject>                         sceneObjects;
        glm::mat4                                        ProjectionMatrix;
        glm::mat4                                        ViewMatrix;
        glm::vec3                                        eyePosition{ 0.0f };
        glm::vec3                                        targetEyePosition{ 0.0f };
        constexpr static float                           ViewAllDistance    = 1.5f;
        constexpr static float                           ViewOneDistance    = 0.6f;
        float                                            viewDistance       = ViewOneDistance;
        float                                            targetViewDistance = ViewAllDistance;

        ObjectModel::Type selectedObjectModel = ObjectModel::Count;
        bool              showWire            = false;
        bool              showNormals         = false;
        bool              showTextured        = true;
        int               stacks              = 20;
        int               slices              = 20;
        bool              autoRotate          = true;
        float             rotationAngle       = 0;

    private:
        void setViewMatrix(glm::vec3 target_position, float distance = 1.5f);
        void drawSceneObjects(const glm::mat4& r, const std::array<graphics::Mesh, ObjectModel::Count>& meshes) const;
        void buildMeshes();
        void buildTriangleMeshes(const std::array<const graphics::Geometry, ObjectModel::Count>& geometries);
        void buildLineMeshes(const std::array<const graphics::Geometry, ObjectModel::Count>& geometries);
        void buildNormalsMeshes(const std::array<const graphics::Geometry, ObjectModel::Count>& geometries);
    };

}
