/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D03Fog.hpp"

#include "environment/Environment.hpp"
#include "graphics/MathHelper.hpp"
#include "opengl/GL.hpp"
#include <glm/ext/matrix_clip_space.hpp> // perspective
#include <glm/ext/matrix_transform.hpp>  // translate, rotate
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp> // all the GLSL trigonometric functions: radians, cos, asin, etc.
#include <imgui.h>

namespace
{
    using namespace std::string_literals;

    namespace Uniforms
    {
        const auto Projection  = "uProjection"s;
        const auto ModelMatrix = "uModelMatrix"s;
        const auto ViewMatrix  = "uViewMatrix"s;
        const auto FogColor    = "uFogColor"s;
        const auto FogNear     = "uFogNear"s;
        const auto FogFar      = "uFogFar"s;
        const auto FogDensity  = "uFogDensity"s;
    }

    namespace asset_paths
    {
        const auto LinearFogVertexPath   = "D03Fog/linear_fog.vert";
        const auto LinearFogFragmentPath = "D03Fog/linear_fog.frag";
        const auto LinearFogShaderName   = "Linear Fog Shader";

        const auto ExponentialFogVertexPath   = "D03Fog/exponential_fog.vert";
        const auto ExponentialFogFragmentPath = "D03Fog/exponential_fog.frag";
        const auto ExponentialFogShaderName   = "Exponential Fog Shader";

        const auto FragCoordZFogVertexPath   = "D03Fog/fragcoordz_fog.vert";
        const auto FragCoordZFogFragmentPath = "D03Fog/fragcoordz_fog.frag";
        const auto FragCoordZFogShaderName   = "FragCoord Z Fog Shader";

        const auto CrateTexturePath    = "D03Fog/crate.jpg";
        const auto PoolBallTexturePath = "D03Fog/pool_ball.jpg";
    }

    namespace camera
    {
        constexpr float     FOV          = glm::radians(60.0f);
        constexpr float     NearDistance = 1.0f;
        constexpr float     FarDistance  = 2'000.0f;
        constexpr glm::vec3 EyePosition{ 0.0f, 0.0f, 2.0f };
        constexpr glm::vec3 RelativeUp{ 0.0f, 1.0f, 0.0f };
        constexpr glm::vec3 TargetPosition{ 0.0f };
    }

}

namespace demos
{
    D03Fog::D03Fog()
    {
        GL::ClearColor(FogColor.r, FogColor.g, FogColor.b, 1.0f);

        ViewMatrix         = glm::lookAt(camera::EyePosition, camera::TargetPosition, camera::RelativeUp);
        const float aspect = static_cast<float>(environment::DisplayWidth) / static_cast<float>(environment::DisplayHeight);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);

        assetReloader.SetAndAutoReloadShader(shaders[FogStyle::Linear], asset_paths::LinearFogShaderName, { asset_paths::LinearFogVertexPath, asset_paths::LinearFogFragmentPath });
        assetReloader.SetAndAutoReloadShader(shaders[FogStyle::Exponential], asset_paths::ExponentialFogShaderName, { asset_paths::ExponentialFogVertexPath, asset_paths::ExponentialFogFragmentPath });
        assetReloader.SetAndAutoReloadShader(shaders[FogStyle::FragCoordZ], asset_paths::FragCoordZFogShaderName, { asset_paths::FragCoordZFogVertexPath, asset_paths::FragCoordZFogFragmentPath });

        assetReloader.SetAndAutoReloadTexture(textures[Materials::Crate], asset_paths::CrateTexturePath);
        textures[Materials::Crate].SetFiltering(GLTexture::Linear);

        assetReloader.SetAndAutoReloadTexture(textures[Materials::PoolBall], asset_paths::PoolBallTexturePath);
        textures[Materials::Crate].SetFiltering(GLTexture::Linear);

        setMaterialsForShader(FogStyle::Linear);

        buildMeshes();

        createLongLineSceneObjects();

        createCirclingSceneObjects();
    }

    void D03Fog::Update()
    {
        assetReloader.Update();

        modelRotations.x += static_cast<float>(-0.4 * environment::DeltaTime);
        modelRotations.y += static_cast<float>(-0.7 * environment::DeltaTime);


        for (auto& object : sceneObjects)
        {
            object.Update();
        }

        constexpr double FUDGE_FACTOR = 0.75;
        const auto       easing       = std::min(static_cast<float>(environment::DeltaTime * FUDGE_FACTOR), 1.0f);
        float            near = 0, far = 0;
        switch (fogStyle)
        {
            case FogStyle::Linear:
                fogNearDistance += easing * (targetFogNearDistance - fogNearDistance);
                fogFarDistance += easing * (targetFogFarDistance - fogFarDistance);
                near = fogNearDistance;
                far  = fogFarDistance;
                break;
            case FogStyle::Exponential:
                fogDensity += easing * (targetFogDensity - fogDensity);
                break;
            case FogStyle::FragCoordZ:
                fragCoordZFogNearDistance += easing * (targetFragCoordZFNearDistance - fragCoordZFogNearDistance);
                fragCoordZFFarDistance += easing * (targetFragCoordZFogFarDistance - fragCoordZFFarDistance);
                near = fragCoordZFogNearDistance;
                far  = fragCoordZFFarDistance;
                break;
            default: break;
        }

        for (auto& material : materials)
        {
            material.SetMaterialUniform(Uniforms::Projection, ProjectionMatrix);
            material.SetMaterialUniform(Uniforms::ViewMatrix, ViewMatrix);
            material.SetMaterialUniform(Uniforms::FogColor, FogColor);
            material.SetMaterialUniform(Uniforms::FogNear, near);
            material.SetMaterialUniform(Uniforms::FogFar, far);
            material.SetMaterialUniform(Uniforms::FogDensity, fogDensity);
        }
    }

    void D03Fog::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (const auto& scene_object : sceneObjects)
        {
            const auto      t            = glm::translate(glm::mat4(1.0f), scene_object.Translation);
            constexpr auto  s            = glm::mat4(1.0f);
            const auto      r            = graphics::euler_angle_xyz_matrix(scene_object.EulerAngles.x, scene_object.EulerAngles.y, scene_object.EulerAngles.z);
            const glm::mat4 ModelMatrix  = t * r * s;
            const auto&     mesh_to_draw = meshesTriangles[scene_object.Model];
            for (const auto& sub_mesh : mesh_to_draw.SubMeshes)
            {
                auto& material = *sub_mesh.Material;
                material.SetMaterialUniform(Uniforms::ModelMatrix, ModelMatrix);
                material.ForceApplyAllSettings();
                sub_mesh.VertexArrayObj.Use();
                GLDrawIndexed(sub_mesh.VertexArrayObj);
            }
        }
        graphics::DEFAULT_MATERIAL.ForceApplyAllSettings();
    }

    void D03Fog::ImGuiDraw()
    {
        constexpr const char* items[] = { "Linear",
                                          "Exponential",
                                          "FragCoordZ" };

        constexpr FogStyle::Type types[]             = { FogStyle::Linear,
                                                         FogStyle::Exponential,
                                                         FogStyle::FragCoordZ };
        auto                     current_focus       = fogStyle;
        const char* const        combo_preview_value = items[current_focus];
        if (ImGui::BeginCombo("Fog Style", combo_preview_value, 0))
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

        if (current_focus != fogStyle)
        {
            fogStyle = current_focus;
            setMaterialsForShader(fogStyle);
        }

        switch (fogStyle)
        {
            case FogStyle::Linear:
                ImGui::SliderFloat("fogNear", &targetFogNearDistance, LinearFogMaxRange.x, LinearFogMaxRange.y);
                ImGui::SliderFloat("fogFar", &targetFogFarDistance, LinearFogMaxRange.x, LinearFogMaxRange.y);
                break;
            case FogStyle::Exponential:
                ImGui::SliderFloat("fogDensity", &targetFogDensity, ExponentialFogMaxRange.x, ExponentialFogMaxRange.y);
                break;
            case FogStyle::FragCoordZ:
                ImGui::SliderFloat("fogNear", &targetFragCoordZFNearDistance, FragCoordZFogMaxRange.x, FragCoordZFogMaxRange.y);
                ImGui::SliderFloat("fogFar", &targetFragCoordZFogFarDistance, FragCoordZFogMaxRange.x, FragCoordZFogMaxRange.y);
                break;
            default: break;
        }
    }

    void D03Fog::SetDisplaySize(int width, int height)
    {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);
    }

    void D03Fog::setMaterialsForShader(FogStyle::Type shaded_type)
    {
        materials[Materials::Crate] = graphics::Material(&shaders[shaded_type], "Crate Material");
        materials[Materials::Crate].SetTextures({ &textures[Materials::Crate] });
        materials[Materials::PoolBall]      = materials[Materials::Crate];
        materials[Materials::PoolBall].Name = "Pool Ball Material";
        materials[Materials::PoolBall].SetTextures({ &textures[Materials::PoolBall] });
    }

    void D03Fog::buildMeshes()
    {
        const std::array<const graphics::Geometry, ObjectModel::Count> geometries = {
            graphics::create_cube(1, 1),
            graphics::create_sphere(40, 40)
        };

        meshesTriangles[ObjectModel::Cube].Name = "Cube";
        meshesTriangles[ObjectModel::Cube].SubMeshes.clear();
        meshesTriangles[ObjectModel::Cube].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Cube], &materials[Materials::Crate]));

        meshesTriangles[ObjectModel::Sphere].Name = "Sphere";
        meshesTriangles[ObjectModel::Sphere].SubMeshes.clear();
        meshesTriangles[ObjectModel::Sphere].SubMeshes.push_back(graphics::to_submesh_as_triangles(geometries[ObjectModel::Sphere], &materials[Materials::PoolBall]));
    }

    void D03Fog::createLongLineSceneObjects()
    {
        constexpr int   NumObjects = 40;
        constexpr float XOffset    = 1.1f;
        constexpr float ZOffset    = -1.4f;
        float           which_obj  = 0;
        for (int i = 0; i < NumObjects; ++i)
        {
            auto& object        = sceneObjects.emplace_back();
            object.Model        = (i & 1) ? ObjectModel::Sphere : ObjectModel::Cube;
            const float index_f = static_cast<float>(i);
            object.Translation  = glm::vec3{ -2.0f + index_f * XOffset, 0.0f, index_f * ZOffset };
            const auto index    = sceneObjects.size() - 1;
            object.Update       = [this, index, which_obj]()
            {
                auto& scene_object         = sceneObjects[index];
                scene_object.EulerAngles.x = modelRotations.x + which_obj * 0.1f;
                scene_object.EulerAngles.y = modelRotations.y + which_obj * 0.1f;
            };
            ++which_obj;
        }
    }

    void D03Fog::createCirclingSceneObjects()
    {
        constexpr int NumObjects = 8;
        for (int i = 0; i < NumObjects; ++i)
        {
            auto&      object      = sceneObjects.emplace_back();
            const auto start_angle = graphics::PI * 2.0f * (static_cast<float>(i) / static_cast<float>(NumObjects));
            object.Model           = (i & 1) ? ObjectModel::Sphere : ObjectModel::Cube;
            const auto index       = sceneObjects.size() - 1;
            object.Update          = [this, index, start_angle]()
            {
                constexpr glm::vec3 AXIS{ 1.0f, 1.0f, 0.0f };
                auto&               scene_object    = sceneObjects[index];
                const glm::mat4     rotate_position = glm::rotate(glm::mat4(1.0f), start_angle + static_cast<float>(environment::ElapsedTime * 0.5), AXIS);
                scene_object.Translation            = glm::vec3((rotate_position * glm::vec4{ 0.0f, 0.0f, 5.0f, 1.0f }));
                const auto look_at                  = glm::lookAt(scene_object.Translation, camera::EyePosition, camera::RelativeUp);
                const auto quat                     = glm::quat_cast(look_at);
                scene_object.EulerAngles            = glm::vec3{ glm::roll(quat), glm::pitch(quat), glm::yaw(quat) + glm::radians(180.0f) };
            };
        }
    }
}
