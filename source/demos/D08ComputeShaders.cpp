/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D08ComputeShaders.hpp"

#include "environment/Environment.hpp"
#include "opengl/GL.hpp"
#include <array>
#include <glm/ext/matrix_clip_space.hpp> // perspective
#include <imgui.h>
#include <span>

namespace
{
    using namespace std::string_literals;

    namespace Uniforms
    {
        const auto Projection = "uProjection"s;
        const auto Time       = "uTime"s;

    }

    namespace asset_paths
    {
        constexpr auto SimpleVertexPath           = "D08ComputeShaders/simple.vert";
        constexpr auto SimpleFragmentPath         = "D08ComputeShaders/simple.frag";
        constexpr auto SimpleComputePath          = "D08ComputeShaders/simple.comp";
        constexpr auto AnimatedTextureComputePath = "D08ComputeShaders/circle_color_pulse.comp";
        constexpr auto LuminanceComputePath       = "D08ComputeShaders/luminance.comp";
        constexpr auto FunComputePath             = "D08ComputeShaders/fun.comp";

        constexpr auto bell_image_path = "D08ComputeShaders/bell.png";


    }
}

namespace demos
{
    D08ComputeShaders::D08ComputeShaders()
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);
        assetReloader.SetAndAutoReloadShader(computeShaders[Example::Simple], "Simple Compute", { asset_paths::SimpleComputePath });
        assetReloader.SetAndAutoReloadShader(computeShaders[Example::Luminance], "Luminance Compute", { asset_paths::LuminanceComputePath });
        assetReloader.SetAndAutoReloadShader(computeShaders[Example::CircleColorPulse], "Circle Color Pulse Compute", { asset_paths::AnimatedTextureComputePath });
        assetReloader.SetAndAutoReloadShader(computeShaders[Example::Fun], "Fun Compute", { asset_paths::FunComputePath });
        assetReloader.SetAndAutoReloadShader(displayTextureShader, "Display Texture Shader", { asset_paths::SimpleVertexPath, asset_paths::SimpleFragmentPath });

        assetReloader.SetAndAutoReloadTexture(bellTexture, asset_paths::bell_image_path);

        if (const bool loaded = generatedTexture.LoadAsRGBA(textureSize, textureSize); !loaded)
        {
            throw std::runtime_error{ "Failed to create 2D texture image in D08ComputeShaders" };
        }

        generatedTexture.SetFiltering(GLTexture::Linear);
        generatedTexture.SetWrapping(GLTexture::ClampToEdge);
        displayTextureMaterial = graphics::Material(&displayTextureShader, "Full Screen Material");
        displayTextureMaterial.SetTextures({ &generatedTexture });

        graphics::Geometry full_screen_geometry = graphics::create_plane(1, 1);
        for (auto& vert : full_screen_geometry.Vertices)
        {
            vert.position *= 2.0f;
        }
        quadMesh = graphics::to_submesh_as_triangles(full_screen_geometry, &displayTextureMaterial);

        D08ComputeShaders::SetDisplaySize(environment::DisplayWidth, environment::DisplayHeight);
    }

    void D08ComputeShaders::Update()
    {
        assetReloader.Update();
        displayTextureMaterial.SetMaterialUniform(Uniforms::Projection, orthoProjectionMatrix);
        updateDispatchSize();
    }

    void D08ComputeShaders::Draw() const
    {
        computeShaders[selectedComputeShader].Use();
#if !defined(OPENGL_ES3_ONLY)
        // TODO GL::BindImageTexture - https://docs.gl/gl4/glBindImageTexture
        //  want to bind generatedTexture to the image2D uniform for writing
        GL::BindImageTexture(0, generatedTexture.GetHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        switch (selectedComputeShader)
        {
            case Example::Simple:
                break;
            case Example::Luminance:
                bellTexture.UseForSlot(1);
                break;
            case Example::CircleColorPulse:
            case Example::Fun:
                computeShaders[selectedComputeShader].SendUniform(Uniforms::Time, static_cast<float>(environment::ElapsedTime));
                break;
            case Example::Count:
                break;
        }
        // TODO GL::DispatchCompute - https://docs.gl/gl4/glDispatchCompute
        GL::DispatchCompute(dispatchSize[0], dispatchSize[1], dispatchSize[2]);

        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // TODO GL::MemoryBarrier - https://docs.gl/gl4/glMemoryBarrier
        //  want to wait till compute shader is done with accessing images, so we can use them for drawing next
        GL::MemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#endif
        displayTextureMaterial.ForceApplyAllSettings();
        quadMesh.VertexArrayObj.Use();
        GLDrawIndexed(quadMesh.VertexArrayObj);
    }

    void D08ComputeShaders::ImGuiDraw()
    {
        constexpr auto example_compute_names = "Simple\0Luminance\0CircleColorPulse\0Fun\0\0";
        ImGui::Combo("Compute Example", reinterpret_cast<int*>(&selectedComputeShader), example_compute_names);
        constexpr int texture_sizes[] = { 256, 512, 1024, 2048, 4096 };
        int           current_pick    = [&](int size)
        {
            int index = 0;
            for (const auto s : texture_sizes)
            {
                if (size == s)
                {
                    return index;
                }
                ++index;
            }
            return 0;
        }(textureSize);

        constexpr auto texture_sizes_names = " 256\0 512\0 1024\0 2048\0 4096\0\0";
        if (ImGui::Combo("Generated Texture Size", &current_pick, texture_sizes_names))
        {
            GLTexture attempt;
            if (const bool loaded = attempt.LoadAsRGBA(texture_sizes[current_pick], texture_sizes[current_pick]); loaded)
            {
                generatedTexture = std::move(attempt);
                textureSize      = texture_sizes[current_pick];
                updateDispatchSize();
            }
        }

        ImGui::LabelText("WorkGroup Size", "%dx%dx%d", localWorkgroupSize[0], localWorkgroupSize[1], localWorkgroupSize[2]);
        ImGui::LabelText("Dispatch Size", "%dx%dx%d", dispatchSize[0], dispatchSize[1], dispatchSize[2]);
    }

    void D08ComputeShaders::SetDisplaySize(int width, int height)
    {
        if (width >= height)
        {
            const float aspect    = static_cast<float>(width) / static_cast<float>(height);
            orthoProjectionMatrix = glm::ortho(-aspect, aspect, -1.0f, 1.0f);
        }
        else
        {
            const float aspect    = static_cast<float>(height) / static_cast<float>(width);
            orthoProjectionMatrix = glm::ortho(-1.0f, 1.0f, -aspect, aspect);
        }
    }

    void D08ComputeShaders::updateDispatchSize()
    {
        // TODO GL::GetProgramiv - https://docs.gl/gl4/glGetProgram
        //  need to get the local workgroup size from the compute shader
        GLint workgroup_size[3];
        GL::GetProgramiv(computeShaders[selectedComputeShader].GetHandle(), GL_COMPUTE_WORK_GROUP_SIZE, workgroup_size);
        localWorkgroupSize = { workgroup_size[0], workgroup_size[1], workgroup_size[2] };

        dispatchSize[0] = gsl::narrow<GLuint>(textureSize / localWorkgroupSize[0]);
        dispatchSize[1] = gsl::narrow<GLuint>(textureSize / localWorkgroupSize[1]);
        dispatchSize[2] = 1;
    }
}
