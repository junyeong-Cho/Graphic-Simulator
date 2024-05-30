/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D11CurvesNSplines.hpp"

#include "environment/Environment.hpp"
#include "environment/Input.hpp"
#include "environment/OpenGL.hpp"

#include "graphics/MathHelper.hpp"

#include "opengl/GL.hpp"

#include <SDL.h>
#include <glm/ext/matrix_clip_space.hpp> // perspective
#include <imgui.h>
#include <span>

namespace
{
    using namespace std::string_literals;

    namespace Uniforms
    {
        const auto Projection      = "uProjection"s;
        const auto ModelMatrix     = "uModelMatrix"s;
        const auto ViewMatrix      = "uViewMatrix"s;
        const auto DiffuseMaterial = "uDiffuseMaterial"s;
    }

    namespace asset_paths
    {
        const auto Fill3DVertexPath   = "D11CurvesNSplines/fill_3d.vert";
        const auto Fill3DFragmentPath = "D11CurvesNSplines/fill_3d.frag";
        const auto Fill3DShaderName   = "Fill 3D Shader";

        const auto Textured3DVertexPath   = "D11CurvesNSplines/texture_3d.vert";
        const auto Textured3DFragmentPath = "D11CurvesNSplines/texture_3d.frag";
        const auto Textured3DShaderName   = "Textured 3D Shader";
        const auto UVTexturePath          = "D11CurvesNSplines/RizomUV_UVGrids/RizomUV_UVGrid_1024x1024.png";
    }

    namespace camera
    {
        constexpr float FOV          = glm::radians(60.0f);
        constexpr float NearDistance = 0.05f;
        constexpr float FarDistance  = 150.0f;
    }

    graphics::SubMesh  to_submesh_of_normals_as_lines(const graphics::Geometry& geometry, graphics::Material* material);
    graphics::Geometry create_line(const std::vector<glm::vec3>& points);
}

namespace demos
{
    D11CurvesNSplines::D11CurvesNSplines()
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);

        assetReloader.SetAndAutoReloadShader(fillShader, asset_paths::Fill3DShaderName, { asset_paths::Fill3DVertexPath, asset_paths::Fill3DFragmentPath });
        assetReloader.SetAndAutoReloadShader(texturedShader, asset_paths::Textured3DShaderName, { asset_paths::Textured3DVertexPath, asset_paths::Textured3DFragmentPath });
        assetReloader.SetAndAutoReloadTexture(uvTexture, asset_paths::UVTexturePath);
        uvTexture.SetFiltering(GLTexture::Linear);

        materials[Materials::Textured] = graphics::Material(&texturedShader, "Textured Objects Material");
        materials[Materials::Textured].SetTextures({ &uvTexture });

        materials[Materials::TexturedPlane]                 = materials[Materials::Textured];
        materials[Materials::TexturedPlane].Name            = "DoubleSided Textured Material";
        materials[Materials::TexturedPlane].Culling.Enabled = false; // draw back facing and front facing triangles

        materials[Materials::Wireframe] = graphics::Material(&fillShader, "Wireframe Material");
        materials[Materials::Wireframe].SetMaterialUniform(Uniforms::DiffuseMaterial, glm::vec3(0.19f, 0.191f, 0.191f));

        materials[Materials::Normals]      = materials[Materials::Wireframe];
        materials[Materials::Normals].Name = "Normals Material";
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
        {
            SceneObject line;
            line.Model       = ObjectModel::Line;
            line.Translation = glm::vec3(0, 0, 0);
            sceneObjects.push_back(line);
        }
    }

    void D11CurvesNSplines::Update()
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
    }

    void D11CurvesNSplines::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const auto      angle = (autoRotate) ? glm::radians(static_cast<float>(environment::ElapsedTime) * 35.0f) : rotationAngle;
        const glm::mat4 r     = glm::rotate(glm::mat4(1.0f), angle, glm::vec3{ 1, 1, 0 });
        if (showNormals)
        {
            drawSceneObjects(r, meshesNormals);
        }
        if (showWire)
        {
            drawSceneObjects(r, meshesLines);
        }
        if (showTextured)
        {
            drawSceneObjects(r, meshesTriangles);
        }
        graphics::DEFAULT_MATERIAL.ForceApplyAllSettings();
    }

    void D11CurvesNSplines::ImGuiDraw()
    {
        constexpr const char* items[] = { "Plane", "Cube", "Sphere", "Torus", "Cylinder", "Cone", "Line", "All" };

        constexpr ObjectModel::Type types[]             = { ObjectModel::Plane,    ObjectModel::Cube, ObjectModel::Sphere, ObjectModel::Torus,
                                                            ObjectModel::Cylinder, ObjectModel::Cone, ObjectModel::Line,   ObjectModel::Count };
        auto                        current_focus       = selectedObjectModel;
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
        ImGui::Checkbox("Textured", &showTextured);
        ImGui::Checkbox("Wire Frame", &showWire);
        ImGui::Checkbox("Show Normals", &showNormals);
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

    void D11CurvesNSplines::SetDisplaySize(int width, int height)
    {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);
    }

    void D11CurvesNSplines::setViewMatrix(glm::vec3 target_position, float distance)
    {
        const glm::vec3     eye_position{ target_position.x, target_position.y, distance };
        constexpr glm::vec3 relative_up{ 0, 1, 0 };
        ViewMatrix = glm::lookAt(eye_position, target_position, relative_up);
    }

    void D11CurvesNSplines::drawSceneObjects(const glm::mat4& r, const std::array<graphics::Mesh, ObjectModel::Count>& meshes) const
    {
        for (const auto& scene_object : sceneObjects)
        {
            const auto      t            = glm::translate(glm::mat4(1.0f), scene_object.Translation);
            const auto      s            = glm::scale(glm::mat4(1.0f), glm::vec3(0.35f));
            const glm::mat4 ModelMatrix  = t * r * s;
            const auto&     mesh_to_draw = meshes[scene_object.Model];
            for (const auto& sub_mesh : mesh_to_draw.SubMeshes)
            {
                auto& material = *sub_mesh.Material;
                material.SetMaterialUniform(Uniforms::ModelMatrix, ModelMatrix);
                material.ForceApplyAllSettings();
                sub_mesh.VertexArrayObj.Use();
                GLDrawIndexed(sub_mesh.VertexArrayObj);
            }
        }
    }

    void D11CurvesNSplines::buildMeshes()
    {
        const std::array<const graphics::Geometry, ObjectModel::Count> geometries = { graphics::create_plane(stacks, slices),
                                                                                      graphics::create_cube(stacks, slices),
                                                                                      graphics::create_sphere(stacks, slices),
                                                                                      graphics::create_torus(stacks, slices),
                                                                                      graphics::create_cylinder(stacks, slices),
                                                                                      graphics::create_cone(stacks, slices),
                                                                                      graphics::create_line({ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) }) };

        buildTriangleMeshes(geometries);
        buildLineMeshes(geometries);
        buildNormalsMeshes(geometries);
    }

    void D11CurvesNSplines::buildTriangleMeshes(const std::array<const graphics::Geometry, ObjectModel::Count>& geometries)
    {
        meshesTriangles[ObjectModel::Plane].Name = "Plane";
        meshesTriangles[ObjectModel::Plane].SubMeshes.clear();
        meshesTriangles[ObjectModel::Plane].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Plane], &materials[Materials::TexturedPlane]));

        meshesTriangles[ObjectModel::Cube].Name = "Cube";
        meshesTriangles[ObjectModel::Cube].SubMeshes.clear();
        meshesTriangles[ObjectModel::Cube].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Cube], &materials[Materials::Textured]));

        meshesTriangles[ObjectModel::Sphere].Name = "Sphere";
        meshesTriangles[ObjectModel::Sphere].SubMeshes.clear();
        meshesTriangles[ObjectModel::Sphere].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Sphere], &materials[Materials::Textured]));

        meshesTriangles[ObjectModel::Torus].Name = "Torus";
        meshesTriangles[ObjectModel::Torus].SubMeshes.clear();
        meshesTriangles[ObjectModel::Torus].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Torus], &materials[Materials::Textured]));

        meshesTriangles[ObjectModel::Cylinder].Name = "Cylinder";
        meshesTriangles[ObjectModel::Cylinder].SubMeshes.clear();
        meshesTriangles[ObjectModel::Cylinder].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Cylinder], &materials[Materials::Textured]));

        meshesTriangles[ObjectModel::Cone].Name = "Cone";
        meshesTriangles[ObjectModel::Cone].SubMeshes.clear();
        meshesTriangles[ObjectModel::Cone].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Cone], &materials[Materials::Textured]));

        meshesTriangles[ObjectModel::Line].Name = "Line";
        meshesTriangles[ObjectModel::Line].SubMeshes.clear();
        meshesTriangles[ObjectModel::Line].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Line], &materials[Materials::Textured]));
    }

    void D11CurvesNSplines::buildLineMeshes(const std::array<const graphics::Geometry, ObjectModel::Count>& geometries)
    {
        meshesLines[ObjectModel::Plane].Name = "Plane as Lines";
        meshesLines[ObjectModel::Plane].SubMeshes.clear();
        meshesLines[ObjectModel::Plane].SubMeshes.push_back(graphics::to_submesh_as_lines(geometries[ObjectModel::Plane], &materials[Materials::Wireframe]));

        meshesLines[ObjectModel::Cube].Name = "Cube as Lines";
        meshesLines[ObjectModel::Cube].SubMeshes.clear();
        meshesLines[ObjectModel::Cube].SubMeshes.push_back(graphics::to_submesh_as_lines(geometries[ObjectModel::Cube], &materials[Materials::Wireframe]));

        meshesLines[ObjectModel::Sphere].Name = "Sphere as Lines";
        meshesLines[ObjectModel::Sphere].SubMeshes.clear();
        meshesLines[ObjectModel::Sphere].SubMeshes.push_back(graphics::to_submesh_as_lines(geometries[ObjectModel::Sphere], &materials[Materials::Wireframe]));

        meshesLines[ObjectModel::Torus].Name = "Torus as Lines";
        meshesLines[ObjectModel::Torus].SubMeshes.clear();
        meshesLines[ObjectModel::Torus].SubMeshes.push_back(graphics::to_submesh_as_lines(geometries[ObjectModel::Torus], &materials[Materials::Wireframe]));

        meshesLines[ObjectModel::Cylinder].Name = "Cylinder as Lines";
        meshesLines[ObjectModel::Cylinder].SubMeshes.clear();
        meshesLines[ObjectModel::Cylinder].SubMeshes.push_back(graphics::to_submesh_as_lines(geometries[ObjectModel::Cylinder], &materials[Materials::Wireframe]));

        meshesLines[ObjectModel::Cone].Name = "Cone as Lines";
        meshesLines[ObjectModel::Cone].SubMeshes.clear();
        meshesLines[ObjectModel::Cone].SubMeshes.push_back(graphics::to_submesh_as_lines(geometries[ObjectModel::Cone], &materials[Materials::Wireframe]));

        meshesLines[ObjectModel::Line].Name = "Line as Lines";
        meshesLines[ObjectModel::Line].SubMeshes.clear();
        meshesLines[ObjectModel::Line].SubMeshes.push_back(graphics::to_submesh_as_lines(geometries[ObjectModel::Line], &materials[Materials::Wireframe]));
    }

    void D11CurvesNSplines::buildNormalsMeshes(const std::array<const graphics::Geometry, ObjectModel::Count>& geometries)
    {
        meshesNormals[ObjectModel::Plane].Name = "Plane as Lines";
        meshesNormals[ObjectModel::Plane].SubMeshes.clear();
        meshesNormals[ObjectModel::Plane].SubMeshes.push_back(to_submesh_of_normals_as_lines(geometries[ObjectModel::Plane], &materials[Materials::Normals]));

        meshesNormals[ObjectModel::Cube].Name = "Cube as Lines";
        meshesNormals[ObjectModel::Cube].SubMeshes.clear();
        meshesNormals[ObjectModel::Cube].SubMeshes.push_back(to_submesh_of_normals_as_lines(geometries[ObjectModel::Cube], &materials[Materials::Normals]));

        meshesNormals[ObjectModel::Sphere].Name = "Sphere as Lines";
        meshesNormals[ObjectModel::Sphere].SubMeshes.clear();
        meshesNormals[ObjectModel::Sphere].SubMeshes.push_back(to_submesh_of_normals_as_lines(geometries[ObjectModel::Sphere], &materials[Materials::Normals]));

        meshesNormals[ObjectModel::Torus].Name = "Torus as Lines";
        meshesNormals[ObjectModel::Torus].SubMeshes.clear();
        meshesNormals[ObjectModel::Torus].SubMeshes.push_back(to_submesh_of_normals_as_lines(geometries[ObjectModel::Torus], &materials[Materials::Normals]));

        meshesNormals[ObjectModel::Cylinder].Name = "Cylinder as Lines";
        meshesNormals[ObjectModel::Cylinder].SubMeshes.clear();
        meshesNormals[ObjectModel::Cylinder].SubMeshes.push_back(to_submesh_of_normals_as_lines(geometries[ObjectModel::Cylinder], &materials[Materials::Normals]));

        meshesNormals[ObjectModel::Cone].Name = "Cone as Lines";
        meshesNormals[ObjectModel::Cone].SubMeshes.clear();
        meshesNormals[ObjectModel::Cone].SubMeshes.push_back(to_submesh_of_normals_as_lines(geometries[ObjectModel::Cone], &materials[Materials::Normals]));

        meshesNormals[ObjectModel::Line].Name = "Line as Normals";
        meshesNormals[ObjectModel::Line].SubMeshes.clear();
        meshesNormals[ObjectModel::Line].SubMeshes.push_back(to_submesh_of_normals_as_lines(geometries[ObjectModel::Line], &materials[Materials::Normals]));
    }
}

namespace
{
    graphics::SubMesh to_submesh_of_normals_as_lines(const graphics::Geometry& geometry, graphics::Material* material)
    {
        GLAttributeLayout position;
        GLAttributeLayout normal;
        GLAttributeLayout uv;
        graphics::describe_meshvertex_layout(position, normal, uv);
        graphics::Geometry normals_geometry;

        unsigned count = 0;
        for (const auto& original_vertex : geometry.Vertices)
        {
            auto PointA = original_vertex;
            PointA.position += PointA.normal * 0.001f;
            auto PointB = PointA;
            PointB.position += PointA.normal * 0.1f;
            normals_geometry.Vertices.push_back(PointA);
            normals_geometry.Vertices.push_back(PointB);
            normals_geometry.Indicies.push_back(count);
            normals_geometry.Indicies.push_back(count + 1);
            count += 2;
        }

        graphics::SubMesh sub_mesh;
        sub_mesh.VertexArrayObj.SetPrimitivePattern(GLPrimitive::Lines);
        sub_mesh.VertexArrayObj.AddVertexBuffer(GLVertexBuffer(std::span{ normals_geometry.Vertices }), { position, normal, uv });
        sub_mesh.VertexArrayObj.SetIndexBuffer(GLIndexBuffer(std::span{ normals_geometry.Indicies }));
        sub_mesh.Material = material;
        return sub_mesh;
    }

    graphics::Geometry create_line(const std::vector<glm::vec3>& points)
    {
        std::vector<graphics::MeshVertex> vertices;
        vertices.reserve(points.size());

        for (const auto& point : points)
        {
            graphics::MeshVertex v;
            v.position = point;

            vertices.push_back(v);
        }

        std::vector<unsigned> indices;
        indices.reserve((points.size() - 1) * 2);

        for (unsigned i = 0; i < points.size() - 1; ++i)
        {
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        return graphics::Geometry{ std::move(vertices), std::move(indices) };
    }
}
