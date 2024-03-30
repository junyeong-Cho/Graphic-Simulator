/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D04ToonShading.hpp"

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
        const auto Projection         = "uProjection"s;
        const auto ModelMatrix        = "uModelMatrix"s;
        const auto ViewMatrix         = "uViewMatrix"s;
        const auto NormalMatrix       = "uNormalMatrix"s;
        const auto FogColor           = "uFogColor"s;
        const auto FogNear            = "uFogNear"s;
        const auto FogFar             = "uFogFar"s;
        const auto Diffuse            = "uDiffuse"s;
        const auto Ambient            = "uAmbient"s;
        const auto Shininess          = "uShininess"s;
        const auto SpecularColor      = "uSpecularColor"s;
        const auto LightDirection     = "uLightDirection"s;
        const auto Level1             = "uLevel1"s;
        const auto Level2             = "uLevel2"s;
        const auto Level3             = "uLevel3"s;
        const auto Max                = "uMax"s;
        const auto NumDiffuseChunks   = "uNumDiffuseChunks"s;
        const auto EnableAntiAliasing = "uEnableAntiAliasing"s;
        const auto ImageWidth         = "uImageWidth"s;

    }

    namespace asset_paths
    {
        const auto CustomRangeToonVertexPath   = "D04ToonShading/toon.vert";
        const auto CustomRangeToonFragmentPath = "D04ToonShading/01_custom_range_toon.frag";
        const auto CustomRangeToonShaderName   = "Custom Range Toon Shader";

        const auto UniformRangeToonVertexPath   = "D04ToonShading/toon.vert";
        const auto UniformRangeToonFragmentPath = "D04ToonShading/02_uniform_range_toon.frag";
        const auto UniformRangeToonShaderName   = "Uniform Range Toon Shader";

        const auto GradientTextureToonVertexPath   = "D04ToonShading/toon.vert";
        const auto GradientTextureToonFragmentPath = "D04ToonShading/03_gradient_texture_toon.frag";
        const auto GradientTextureToonShaderName   = "Gradient Texture Toon Shader";

        const auto ColoredGradient01 = "D04ToonShading/colored_gradient_01.png";
        const auto ColoredGradient02 = "D04ToonShading/colored_gradient_02.png";
        const auto FiveTone          = "D04ToonShading/fiveTone.png";
        const auto ThreeTone         = "D04ToonShading/threeTone.png";

    }

    namespace camera
    {
        constexpr float FOV          = glm::radians(60.0f);
        constexpr float NearDistance = 0.05f;
        constexpr float FarDistance  = 15.0f;
    }

}

namespace demos
{
    D04ToonShading::D04ToonShading()
    {
        GL::ClearColor(FogColor.r, FogColor.g, FogColor.b, 1.0f);

        using namespace asset_paths;
        assetReloader.SetAndAutoReloadShader(shaders[ToonStyle::CustomRange], CustomRangeToonShaderName, { CustomRangeToonVertexPath, CustomRangeToonFragmentPath });
        assetReloader.SetAndAutoReloadShader(shaders[ToonStyle::UniformRange], UniformRangeToonShaderName, { UniformRangeToonVertexPath, UniformRangeToonFragmentPath });
        assetReloader.SetAndAutoReloadShader(shaders[ToonStyle::GradientTexture], GradientTextureToonShaderName, { GradientTextureToonVertexPath, GradientTextureToonFragmentPath });

        assetReloader.SetAndAutoReloadTexture(gradientTextures[Gradients::Colored01], ColoredGradient01);
        assetReloader.SetAndAutoReloadTexture(gradientTextures[Gradients::Colored02], ColoredGradient02);
        assetReloader.SetAndAutoReloadTexture(gradientTextures[Gradients::FiveTone], FiveTone);
        assetReloader.SetAndAutoReloadTexture(gradientTextures[Gradients::ThreeTone], ThreeTone);
        for (auto& texture : gradientTextures)
        {
            texture.SetWrapping(GLTexture::ClampToEdge);
            texture.SetFiltering(GLTexture::NearestPixel);
        }

        materials[ToonStyle::CustomRange]     = graphics::Material(&shaders[ToonStyle::CustomRange], "Custom Range Toon Material");
        materials[ToonStyle::UniformRange]    = graphics::Material(&shaders[ToonStyle::UniformRange], "Uniform Rage Toon Material");
        materials[ToonStyle::GradientTexture] = graphics::Material(&shaders[ToonStyle::GradientTexture], "Gradient Toon Material");
        materials[ToonStyle::GradientTexture].SetTextures({ &gradientTextures[gradientType] });
        materials[ToonStyle::GradientTexture].SetMaterialUniform(Uniforms::ImageWidth, gradientTextures[gradientType].GetWidth());

        buildMeshes();

        const float aspect = static_cast<float>(environment::DisplayWidth) / static_cast<float>(environment::DisplayHeight);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);

        float           angle     = 0;
        constexpr float increment = (2.0f * graphics::PI) / static_cast<float>(ObjectModel::Count);
        for (auto& preset : camPresets)
        {
            constexpr float SCALE = 3.0f;
            preset.center         = glm::vec3{
                0.0f, SCALE * std::sin(angle), SCALE * std::cos(angle)
            };
            angle += increment;
        }

        camPresets[ObjectModel::Cube].scale     = 0.4f;
        camPresets[ObjectModel::Cylinder].scale = 0.5f;

        setLookAt();
    }

    void D04ToonShading::Update()
    {
        assetReloader.Update();

        constexpr double FUDGE_FACTOR = 0.75;
        const auto       easing       = std::min(static_cast<float>(environment::DeltaTime * FUDGE_FACTOR), 1.0f);
        eyePosition += easing * (targetEyePosition - eyePosition);
        lightDirection += easing * (targetLightDirection - lightDirection);
        {
            constexpr glm::vec3 relative_up{ 0, -1, 0 };
            ViewMatrix = glm::lookAt(eyePosition, eyePosition + glm::vec3{ 0.0f, 0.0f, 1.0f }, relative_up);
        }

        for (auto& m : materials)
        {
            m.SetMaterialUniform(Uniforms::Projection, ProjectionMatrix);
            m.SetMaterialUniform(Uniforms::ViewMatrix, ViewMatrix);
            m.SetMaterialUniform(Uniforms::FogColor, FogColor);
            m.SetMaterialUniform(Uniforms::FogNear, fogNearDistance);
            m.SetMaterialUniform(Uniforms::FogFar, fogFarDistance);
            m.SetMaterialUniform(Uniforms::Diffuse, diffuse);
            m.SetMaterialUniform(Uniforms::Ambient, ambient);
            m.SetMaterialUniform(Uniforms::Shininess, shininess);
            m.SetMaterialUniform(Uniforms::SpecularColor, specular);
            m.SetMaterialUniform(Uniforms::LightDirection, lightDirection);
            m.SetMaterialUniform(Uniforms::EnableAntiAliasing, enableAntialising);
        }

        if (gradientType == Gradients::Colored01 || gradientType == Gradients::Colored02)
        {
            // for colored gradients let's use their color
            materials[ToonStyle::GradientTexture].SetMaterialUniform(Uniforms::Diffuse, glm::vec3(1.0f));
            materials[ToonStyle::GradientTexture].SetMaterialUniform(Uniforms::SpecularColor, glm::vec3(1.0f));
        }

        materials[ToonStyle::GradientTexture].SetMaterialUniform(Uniforms::ImageWidth, gradientTextures[gradientType].GetWidth());

        {
            auto& m = materials[ToonStyle::CustomRange];
            m.SetMaterialUniform(Uniforms::Level1, customRangeSettings.level1);
            m.SetMaterialUniform(Uniforms::Level2, customRangeSettings.level2);
            m.SetMaterialUniform(Uniforms::Level3, customRangeSettings.level3);
            m.SetMaterialUniform(Uniforms::Max, customRangeSettings.max);
        }

        materials[ToonStyle::UniformRange].SetMaterialUniform(Uniforms::NumDiffuseChunks, uniformRangeSettings.diffuseChunks);
    }

    void D04ToonShading::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const auto       angle          = (autoRotate) ? glm::radians(static_cast<float>(environment::ElapsedTime) * 35.0f) : rotationAngle;
        glm::mat4        r              = glm::rotate(glm::mat4(1.0f), angle, glm::vec3{ 1, 1, 0 });
        const std::array models_to_draw = { previousObjectModel, selectedObjectModel };
        for (auto model : models_to_draw)
        {
            const auto center = camPresets[model].center;

            drawObject(glm::translate(glm::mat4(1.0f), center + glm::vec3{ -XOffset, 0.0f, 0.0f }), r, model, ToonStyle::CustomRange);

            r = glm::rotate(glm::mat4(1.0f), angle + 0.2f, glm::vec3{ 1, 1, 0 });
            drawObject(glm::translate(glm::mat4(1.0f), center), r, model, ToonStyle::UniformRange);

            r = glm::rotate(glm::mat4(1.0f), angle + 0.4f, glm::vec3{ 1, 1, 0 });
            drawObject(glm::translate(glm::mat4(1.0f), glm::vec3{ XOffset, 0.0f, 0.0f } + center), r, model, ToonStyle::GradientTexture);
        }
        graphics::DEFAULT_MATERIAL.ForceApplyAllSettings();
    }

    void D04ToonShading::ImGuiDraw()
    {
        if (ImGui::CollapsingHeader("View"))
        {
            {
                constexpr const char* items[] = { "Custom Ranges Toon Shading",
                                                  "Uniform Ranges Toon Shading",
                                                  "Gradient Texture Toon Shading",
                                                  "View All Styles" };

                constexpr ToonStyle::Type types[] = {
                    ToonStyle::CustomRange,
                    ToonStyle::UniformRange,
                    ToonStyle::GradientTexture,
                    ToonStyle::All
                };
                auto              current_focus       = focus;
                const char* const combo_preview_value = items[current_focus];
                if (ImGui::BeginCombo("Focus On Style", combo_preview_value, 0))
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

                if (current_focus != focus)
                {
                    focus = current_focus;
                    setLookAt();
                }
            }
            {
                constexpr const char* items[] = { "Trefoil",
                                                  "Plane",
                                                  "Cube",
                                                  "Sphere",
                                                  "Torus",
                                                  "Cylinder",
                                                  "Cone" };

                constexpr ObjectModel::Type types[] = {
                    ObjectModel::Trefoil,
                    ObjectModel::Plane,
                    ObjectModel::Cube,
                    ObjectModel::Sphere,
                    ObjectModel::Torus,
                    ObjectModel::Cylinder,
                    ObjectModel::Cone
                };
                auto              current_focus       = selectedObjectModel;
                const char* const combo_preview_value = items[current_focus];
                if (ImGui::BeginCombo("Apply to Shape", combo_preview_value, 0))
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
                    previousObjectModel = selectedObjectModel;
                    selectedObjectModel = current_focus;
                    setLookAt();
                }
            }

            ImGui::Checkbox("Enable Antialiasing", &enableAntialising);
            ImGui::Checkbox("Auto Rotate", &autoRotate);
            if (!autoRotate)
            {
                ImGui::SliderAngle("Angle", &rotationAngle);
            }
        }

        if (ImGui::CollapsingHeader("Material"))
        {
            float space[3] = { diffuse.r, diffuse.g, diffuse.b };
            ImGui::ColorEdit3("Diffuse", space);
            diffuse = glm::vec3{ space[0], space[1], space[2] };

            space[0] = specular.r, space[1] = specular.g, space[2] = specular.b;
            ImGui::ColorEdit3("Specular", space);
            specular = glm::vec3{ space[0], space[1], space[2] };

            space[0] = ambient.r, space[1] = ambient.g, space[2] = ambient.b;
            ImGui::ColorEdit3("Ambient", space);
            ambient = glm::vec3{ space[0], space[1], space[2] };

            ImGui::SliderFloat("Shininess", &shininess, 1.0f, 150.0f);

            space[0] = -targetLightDirection.x, space[1] = -targetLightDirection.y, space[2] = -targetLightDirection.z;
            ImGui::InputFloat3("Light Direction", space);
            targetLightDirection = glm::vec3{ -space[0], -space[1], -space[2] };
        }

        if (ImGui::CollapsingHeader("Custom Range Settings"))
        {
            ImGui::SliderFloat("level 1", &customRangeSettings.level1, 0.0f, customRangeSettings.level2);
            ImGui::SliderFloat("level 2", &customRangeSettings.level2, customRangeSettings.level1, customRangeSettings.level3);
            ImGui::SliderFloat("level 3", &customRangeSettings.level3, customRangeSettings.level2, customRangeSettings.max);
            ImGui::SliderFloat("max", &customRangeSettings.max, customRangeSettings.level3, 1.0f);
        }
        if (ImGui::CollapsingHeader("Uniform Range Settings"))
        {
            ImGui::SliderInt("Num Diffuse Chunks", &uniformRangeSettings.diffuseChunks, 1, 20);
        }
        if (ImGui::CollapsingHeader("Gradient Texture Settings"))
        {
            constexpr std::array names = { "Colored 01", "Colored 02", "Five Tone", "Three Tone" };
            const std::array     tints = {
                ImVec4{        1,         1,         1, 1},
                ImVec4{        1,         1,         1, 1},
                ImVec4{diffuse.r, diffuse.g, diffuse.b, 1},
                ImVec4{diffuse.r, diffuse.g, diffuse.b, 1}
            };
            for (size_t i = 0; i < Gradients::Count; ++i)
            {
                if (ImGui::Selectable(names[i], i == gradientType))
                {
                    gradientType = static_cast<Gradients::Type>(i);
                    materials[ToonStyle::GradientTexture].SetTextures({ &gradientTextures[gradientType] });
                }
                ImGui::SameLine();
                const auto&       texture        = gradientTextures[i];
                const ImTextureID handle         = reinterpret_cast<void*>(static_cast<intptr_t>(texture.GetHandle()));
                const float       aspect         = static_cast<float>(texture.GetWidth()) / static_cast<float>(texture.GetHeight());
                constexpr float   display_height = 10.0f;
                const float       display_width  = display_height * aspect;
                ImGui::Image(handle, { display_width, display_height }, { 0, 0 }, { 1, 1 }, tints[i]);
            }
        }
    }

    void D04ToonShading::SetDisplaySize(int width, int height)
    {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);
    }

    void D04ToonShading::setLookAt()
    {
        glm::vec3 center = camPresets[selectedObjectModel].center;
        switch (focus)
        {
            case ToonStyle::CustomRange:
                center.x -= XOffset;
                break;

            case ToonStyle::GradientTexture:
                center.x += XOffset;
                break;
            default: break;
        }
        targetEyePosition = center - glm::vec3{ 0, 0, (focus == ToonStyle::All) ? ViewAllDistance : ViewOneDistance };
    }

    void D04ToonShading::buildMeshes()
    {
        std::array geometries = {
            graphics::create_trefoil(256, 64),
            graphics::create_plane(1, 1),
            graphics::create_cube(1, 1),
            graphics::create_sphere(64, 64),
            graphics::create_torus(64, 64),
            graphics::create_cylinder(4, 64),
            graphics::create_cone(4, 64)
        };

        for (size_t i = 0; i < meshes.size(); ++i)
        {
            auto&      mesh_variation                   = meshes[i];
            const auto num_str                          = std::to_string(i);
            mesh_variation[ToonStyle::CustomRange].Name = "Custom Range Toon Shaded " + num_str;
            mesh_variation[ToonStyle::CustomRange].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[i], &materials[ToonStyle::CustomRange]));
            mesh_variation[ToonStyle::UniformRange].Name = "Uniform Range Toon Shaded " + num_str;
            mesh_variation[ToonStyle::UniformRange].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[i], &materials[ToonStyle::UniformRange]));
            mesh_variation[ToonStyle::GradientTexture].Name = "Gradient Texture Toon Shaded " + num_str;
            mesh_variation[ToonStyle::GradientTexture].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[i], &materials[ToonStyle::GradientTexture]));
        }

        // Let's add another mesh for the plane, so we see something on both sides
        const glm::mat3 flip_plane = glm::mat3(rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3{ 1, 0, 0 }));
        for (auto& vert : geometries[ObjectModel::Plane].Vertices)
        {
            vert.position = flip_plane * vert.position;
            vert.normal   = flip_plane * vert.normal;
        }
        {
            constexpr size_t i                          = ObjectModel::Plane;
            auto&            mesh_variation             = meshes[i];
            const auto       num_str                    = std::to_string(i) + " flipped";
            mesh_variation[ToonStyle::CustomRange].Name = "Custom Range Toon Shaded " + num_str;
            mesh_variation[ToonStyle::CustomRange].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[i], &materials[ToonStyle::CustomRange]));
            mesh_variation[ToonStyle::UniformRange].Name = "Uniform Range Toon Shaded " + num_str;
            mesh_variation[ToonStyle::UniformRange].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[i], &materials[ToonStyle::UniformRange]));
            mesh_variation[ToonStyle::GradientTexture].Name = "Gradient Texture Toon Shaded " + num_str;
            mesh_variation[ToonStyle::GradientTexture].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[i], &materials[ToonStyle::GradientTexture]));
        }
    }

void D04ToonShading::drawObject(const glm::mat4& t, const glm::mat4& r, ObjectModel::Type model, ToonStyle::Type mesh_type) const
    {
        const glm::mat4 s            = glm::scale(glm::mat4(1.0f), glm::vec3(camPresets[model].scale));
        const glm::mat4 ModelMatrix  = t * r * s;
        const glm::mat3 NormalMatrix = glm::transpose(glm::inverse(glm::mat3(ViewMatrix * ModelMatrix)));
        const auto&     mesh_to_draw = meshes[model][mesh_type];

        for (const auto& sub_mesh : mesh_to_draw.SubMeshes)
        {
            auto& m = *sub_mesh.Material;
            m.SetMaterialUniform(Uniforms::ModelMatrix, ModelMatrix);
            m.SetMaterialUniform(Uniforms::NormalMatrix, NormalMatrix);
            m.ForceApplyAllSettings();
            sub_mesh.VertexArrayObj.Use();
            GLDrawIndexed(sub_mesh.VertexArrayObj);
        }
    }
}
