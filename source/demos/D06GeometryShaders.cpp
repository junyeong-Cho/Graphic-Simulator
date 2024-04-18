/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D06GeometryShaders.hpp"

#include "environment/Environment.hpp"
#include "opengl/GL.hpp"
#include <glm/ext/matrix_clip_space.hpp> // perspective
#include <glm/ext/matrix_transform.hpp>  // translate, rotate
#include <glm/trigonometric.hpp>         // all the GLSL trigonometric functions: radians, cos, asin, etc.
#include <imgui.h>

namespace
{
    using namespace std::string_literals;

    namespace Uniforms
    {
        const auto Projection      = "uProjection"s;
        const auto ModelMatrix     = "uModelMatrix"s;
        const auto ViewMatrix      = "uViewMatrix"s;
        const auto DiffuseMaterial = "uDiffuseMaterial"s;
        const auto Time            = "uTime"s;
        const auto ExtrudeFactor   = "uExtrudeFactor";
        const auto Flat            = "uFlat";
        const auto ShrinkFactor    = "uShrinkFactor";
    }

    namespace asset_paths
    {
        const auto UVTexturePath = "D06GeometryShaders/RizomUV_UVGrids/Basic UV Grid 1024x1024_Gradient 2.jpg";

        const auto TexturedShaderName  = "Textured Shader";
        const auto WireframeShaderName = "Wireframe Shader";
        const auto NormalsShaderName   = "Normals Shader";
        const auto ExtrudeShaderName   = "Extrude Shader";
        const auto ShrinkShaderName    = "Shrink Shader";
        const auto FunShaderName       = "Fun Shader";

        const auto Fill3DVertexPath   = "D06GeometryShaders/fill_3d.vert";
        const auto Fill3DFragmentPath = "D06GeometryShaders/fill_3d.frag";


        const auto Textured3DVertexPath   = "D06GeometryShaders/texture_3d.vert";
        const auto Textured3DFragmentPath = "D06GeometryShaders/texture_3d.frag";

        const auto TexturedGeometryPath  = "D06GeometryShaders/default.geom";
        const auto WireframeGeometryPath = "D06GeometryShaders/wireframe.geom";
        const auto NormalsGeometryPath   = "D06GeometryShaders/normals.geom";
        const auto ExtrudeGeometryPath   = "D06GeometryShaders/extrude.geom";
        const auto ExtrudeFragmentPath   = "D06GeometryShaders/extrude.frag";
        const auto ShrinkGeometryPath    = "D06GeometryShaders/shrink.geom";
        const auto ShrinkFragmentPath    = "D06GeometryShaders/shrink.frag";
        const auto FunGeometryPath       = "D06GeometryShaders/fun.geom";

    }

    namespace camera
    {
        constexpr float FOV          = glm::radians(60.0f);
        constexpr float NearDistance = 0.05f;
        constexpr float FarDistance  = 150.0f;
    }
}

namespace demos
{
    D06GeometryShaders::D06GeometryShaders()
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);

        struct
        {
            gsl::czstring                        ShaderName;
            std::array<std::filesystem::path, 3> ShaderFilePaths;
        } shader_setup[Materials::Count] = {
            { asset_paths::TexturedShaderName, { asset_paths::Textured3DVertexPath, asset_paths::TexturedGeometryPath, asset_paths::Textured3DFragmentPath }},
            {asset_paths::WireframeShaderName,        { asset_paths::Fill3DVertexPath, asset_paths::WireframeGeometryPath, asset_paths::Fill3DFragmentPath }},
            {  asset_paths::NormalsShaderName,          { asset_paths::Fill3DVertexPath, asset_paths::NormalsGeometryPath, asset_paths::Fill3DFragmentPath }},
            {  asset_paths::ExtrudeShaderName,     { asset_paths::Textured3DVertexPath, asset_paths::ExtrudeGeometryPath, asset_paths::ExtrudeFragmentPath }},
            {   asset_paths::ShrinkShaderName,           { asset_paths::Fill3DVertexPath, asset_paths::ShrinkGeometryPath, asset_paths::ShrinkFragmentPath }},
            {      asset_paths::FunShaderName,      { asset_paths::Textured3DVertexPath, asset_paths::FunGeometryPath, asset_paths::Textured3DFragmentPath }}
        };

        for (size_t i = 0; i < Materials::Count; ++i)
        {
            const auto& setup = shader_setup[i];
            const auto  paths = std::span{ setup.ShaderFilePaths };
            assetReloader.SetAndAutoReloadShader(shaders[i], setup.ShaderName, paths);
            materials[i]                 = graphics::Material(&shaders[i], setup.ShaderName + " Material"s);
            materials[i].Culling.Enabled = false;
        }

        assetReloader.SetAndAutoReloadTexture(uvTexture, asset_paths::UVTexturePath);
        uvTexture.SetFiltering(GLTexture::Linear);

        materials[Materials::Textured].SetTextures({ &uvTexture });
        materials[Materials::Extrude].SetTextures({ &uvTexture });
        materials[Materials::Fun].SetTextures({ &uvTexture });

        materials[Materials::Wireframe].SetMaterialUniform(Uniforms::DiffuseMaterial, glm::vec3(0.19f, 0.191f, 0.191f));

        materials[Materials::Normals].SetMaterialUniform(Uniforms::DiffuseMaterial, glm::vec3(0.9f, 0.91f, 0.91f));

        buildMeshes();

        const float aspect = static_cast<float>(environment::DisplayWidth) / static_cast<float>(environment::DisplayHeight);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);

        {
            SceneObject plane;
            plane.Model       = ObjectModel::Plane;
            plane.Translation = glm::vec3(0, 0, 0);
            sceneObjects.push_back(plane);
        }
        {
            SceneObject cube;
            cube.Model       = ObjectModel::Cube;
            cube.Translation = glm::vec3(-0.6f, 0.15f, 0.0f);
            sceneObjects.push_back(cube);
        }
        {
            SceneObject sphere;
            sphere.Model       = ObjectModel::Sphere;
            sphere.Translation = glm::vec3(0.6f, 0.15f, 0);
            sceneObjects.push_back(sphere);
        }
        {
            SceneObject torus;
            torus.Model       = ObjectModel::Torus;
            torus.Translation = glm::vec3(-0.5f, -0.5f, 0);
            sceneObjects.push_back(torus);
        }
        {
            SceneObject column;
            column.Model       = ObjectModel::Cylinder;
            column.Translation = glm::vec3(0, 0.5f, 0);
            sceneObjects.push_back(column);
        }
        {
            SceneObject cone;
            cone.Model       = ObjectModel::Cone;
            cone.Translation = glm::vec3(0.5f, -0.5f, 0);
            sceneObjects.push_back(cone);
        }
    }

    void D06GeometryShaders::Update()
    {
        assetReloader.Update();

        constexpr double FUDGE_FACTOR = 1.25;
        const auto       easing       = std::min(static_cast<float>(environment::DeltaTime * FUDGE_FACTOR), 1.0f);
        eyePosition += easing * (targetEyePosition - eyePosition);
        viewDistance += easing * (targetViewDistance - viewDistance);
        setViewMatrix(eyePosition, viewDistance);

        for (auto& material : materials)
        {
            material.SetMaterialUniform(Uniforms::Projection, ProjectionMatrix);
            material.SetMaterialUniform(Uniforms::ViewMatrix, ViewMatrix);
        }

        materials[Materials::Fun].SetMaterialUniform(Uniforms::Time, static_cast<float>(environment::ElapsedTime));
        materials[Materials::Extrude].SetMaterialUniform(Uniforms::ExtrudeFactor, extrudeFactor);
        materials[Materials::Extrude].SetMaterialUniform(Uniforms::Flat, extrudeFlatly);
        materials[Materials::Shrink].SetMaterialUniform(Uniforms::ShrinkFactor, shrinkFactor);
    }

    void D06GeometryShaders::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const auto      angle = (autoRotate) ? glm::radians(static_cast<float>(environment::ElapsedTime) * 35.0f) : rotationAngle;
        const glm::mat4 r     = glm::rotate(glm::mat4(1.0f), angle, glm::vec3{ 1, 1, 0 });


        if (currentMaterial == Materials::Normals)
        {
            drawSceneObjects(r, materials[Materials::Wireframe]);
            drawSceneObjects(r, materials[Materials::Normals]);
        }
        else
        {
            drawSceneObjects(r, materials[currentMaterial]);
        }
        graphics::DEFAULT_MATERIAL.ForceApplyAllSettings();
    }

    void D06GeometryShaders::ImGuiDraw()
    {
        constexpr auto display_items = "Textured\0Generate Wireframe\0Generate Normals\0Extrude Geometry\0Shrink Geometry\0Fun Example\0\0";
        ImGui::Combo("Geometry Shader", reinterpret_cast<int*>(&currentMaterial), display_items);

        switch (currentMaterial)
        {
            case Materials::Textured: break;
            case Materials::Wireframe: break;
            case Materials::Normals: break;
            case Materials::Extrude:
                ImGui::Checkbox("Extrude Flatly", &extrudeFlatly);
                ImGui::SliderFloat("Extrude Factor", &extrudeFactor, 0.0f, 2.0f);
                break;
            case Materials::Shrink: ImGui::SliderFloat("Shrink Factor", &shrinkFactor, 0.0f, 1.0f); break;
            case Materials::Fun: break;
            case Materials::Count: break;
            default:;
        }

        constexpr const char* items[] = { "Plane", "Cube", "Sphere", "Torus", "Cylinder", "Cone", "All" };

        constexpr ObjectModel::Type types[]       = { ObjectModel::Plane, ObjectModel::Cube, ObjectModel::Sphere, ObjectModel::Torus, ObjectModel::Cylinder, ObjectModel::Cone, ObjectModel::Count };
        auto                        current_focus = selectedObjectModel;
        const char* const           combo_preview_value = items[current_focus];
        if (ImGui::BeginCombo("Focus", combo_preview_value, 0))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (current_focus == n);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    current_focus = types[n];
                }
                if (is_selected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (current_focus != selectedObjectModel)
        {
            selectedObjectModel = current_focus;
            if (selectedObjectModel == ObjectModel::Count)
            {
                targetEyePosition  = glm::vec3(0.0f);
                targetViewDistance = ViewAllDistance;
            }
            else
            {
                targetEyePosition  = sceneObjects[selectedObjectModel].Translation;
                targetViewDistance = ViewOneDistance;
            }
        }

        bool rebuild_shapes = ImGui::SliderInt("Stacks", &stacks, 1, 200);
        rebuild_shapes      = ImGui::SliderInt("Slices", &slices, 1, 200) || rebuild_shapes;
        if (rebuild_shapes)
        {
            buildMeshes();
        }
        ImGui::Checkbox("Auto Rotate", &autoRotate);
        if (!autoRotate)
        {
            ImGui::SliderAngle("Angle", &rotationAngle);
        }
    }

    void D06GeometryShaders::SetDisplaySize(int width, int height)
    {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);
    }

    void D06GeometryShaders::setViewMatrix(glm::vec3 target_position, float distance)
    {
        const glm::vec3     eye_position{ target_position.x, target_position.y, distance };
        constexpr glm::vec3 relative_up{ 0, 1, 0 };
        ViewMatrix = glm::lookAt(eye_position, target_position, relative_up);
    }

    void D06GeometryShaders::drawSceneObjects(const glm::mat4& r, graphics::Material& material) const
    {
        for (const auto& scene_object : sceneObjects)
        {
            const auto      t            = glm::translate(glm::mat4(1.0f), scene_object.Translation);
            const auto      s            = glm::scale(glm::mat4(1.0f), glm::vec3(0.35f));
            const glm::mat4 ModelMatrix  = t * r * s;
            const auto&     mesh_to_draw = meshes[scene_object.Model];
            for (const auto& sub_mesh : mesh_to_draw.SubMeshes)
            {
                material.SetMaterialUniform(Uniforms::ModelMatrix, ModelMatrix);
                material.ForceApplyAllSettings();
                sub_mesh.VertexArrayObj.Use();
                GLDrawIndexed(sub_mesh.VertexArrayObj);
            }
        }
    }

    void D06GeometryShaders::buildMeshes()
    {
        const std::array<const graphics::Geometry, ObjectModel::Count> geometries = { graphics::create_plane(stacks, slices),    graphics::create_cube(stacks, slices),
                                                                                      graphics::create_sphere(stacks, slices),   graphics::create_torus(stacks, slices),
                                                                                      graphics::create_cylinder(stacks, slices), graphics::create_cone(stacks, slices) };

        meshes[ObjectModel::Plane].Name = "Plane";
        meshes[ObjectModel::Plane].SubMeshes.clear();
        meshes[ObjectModel::Plane].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Plane]));

        meshes[ObjectModel::Cube].Name = "Cube";
        meshes[ObjectModel::Cube].SubMeshes.clear();
        meshes[ObjectModel::Cube].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Cube]));

        meshes[ObjectModel::Sphere].Name = "Sphere";
        meshes[ObjectModel::Sphere].SubMeshes.clear();
        meshes[ObjectModel::Sphere].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Sphere]));

        meshes[ObjectModel::Torus].Name = "Torus";
        meshes[ObjectModel::Torus].SubMeshes.clear();
        meshes[ObjectModel::Torus].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Torus]));

        meshes[ObjectModel::Cylinder].Name = "Cylinder";
        meshes[ObjectModel::Cylinder].SubMeshes.clear();
        meshes[ObjectModel::Cylinder].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Cylinder]));

        meshes[ObjectModel::Cone].Name = "Cone";
        meshes[ObjectModel::Cone].SubMeshes.clear();
        meshes[ObjectModel::Cone].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Cone]));
    }

}
