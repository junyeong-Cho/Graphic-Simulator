/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "D07TessellationShaders.hpp"

#include "environment/Environment.hpp"
#include "environment/OpenGL.hpp"
#include "opengl/GL.hpp"
#include <array>
#include <glm/ext/matrix_clip_space.hpp> // perspective
#include <glm/ext/matrix_transform.hpp>  // translate, rotate
#include <glm/trigonometric.hpp>         // all the GLSL trigonometric functions: radians, cos, asin, etc.
#include <imgui.h>
#include <span>

#include "graphics/Mesh.hpp"


using namespace std::string_literals;

namespace
{
    namespace Uniforms
    {
        const auto Projection          = "uProjection"s;
        const auto ModelMatrix         = "uModelMatrix"s;
        const auto ViewMatrix          = "uViewMatrix"s;
        const auto DiffuseMaterial     = "uDiffuseMaterial"s;
        const auto OutsideTessellation = "uOutsideTessellation"s;
        const auto InsideTessellation  = "uInsideTessellation"s;
    }

    namespace asset_paths
    {
        constexpr auto ParamsVertexPath               = "D07TessellationShaders/params.vert";
        constexpr auto ParamsQuadControlPath          = "D07TessellationShaders/params_quad.tesc";
        constexpr auto ParamsQuadSpacingEqualEvalPath = "D07TessellationShaders/params_quad_spacing_equal.tese";
        constexpr auto ParamsQuadSpacingEvenEvalPath  = "D07TessellationShaders/params_quad_spacing_even.tese";
        constexpr auto ParamsQuadSpacingOddEvalPath   = "D07TessellationShaders/params_quad_spacing_odd.tese";
        constexpr auto ParamsTriControlPath           = "D07TessellationShaders/params_tri.tesc";
        constexpr auto ParamsTriSpacingEqualEvalPath  = "D07TessellationShaders/params_tri_spacing_equal.tese";
        constexpr auto ParamsTriSpacingEvenEvalPath   = "D07TessellationShaders/params_tri_spacing_even.tese";
        constexpr auto ParamsTriSpacingOddEvalPath    = "D07TessellationShaders/params_tri_spacing_odd.tese";
        constexpr auto ParamsLineControlPath          = "D07TessellationShaders/params_line.tesc";
        constexpr auto ParamsLineSpacingEqualEvalPath = "D07TessellationShaders/params_line_spacing_equal.tese";
        constexpr auto ParamsLineSpacingEvenEvalPath  = "D07TessellationShaders/params_line_spacing_even.tese";
        constexpr auto ParamsLineSpacingOddEvalPath   = "D07TessellationShaders/params_line_spacing_odd.tese";
        constexpr auto WireframeGeometryPath          = "D07TessellationShaders/wireframe.geom";
        constexpr auto PointsGeometryPath             = "D07TessellationShaders/points.geom";
        constexpr auto Fill3DFragmentPath             = "D07TessellationShaders/fill_3d.frag";

        constexpr auto QuadSpacingEqualShaderName = "quad equal spacing shader";
        constexpr auto QuadSpacingEvenShaderName  = "quad even spacing shader";
        constexpr auto QuadSpacingOddShaderName   = "quad odd spacing shader";
        constexpr auto TriSpacingEqualShaderName  = "tri equal spacing shader";
        constexpr auto TriSpacingEvenShaderName   = "tri even spacing shader";
        constexpr auto TriSpacingOddShaderName    = "tri odd spacing shader";
        constexpr auto LineSpacingEqualShaderName = "line equal spacing shader";
        constexpr auto LineSpacingEvenShaderName  = "line even spacing shader";
        constexpr auto LineSpacingOddShaderName   = "line odd spacing shader";
    }

    namespace camera
    {
        constexpr float     FOV          = glm::radians(45.0f);
        constexpr float     NearDistance = 0.1f;
        constexpr float     FarDistance  = 50.0f;
        constexpr glm::vec3 EyePosition{ -2.0f, 2.0f, -2.0f };
        constexpr glm::vec3 RelativeUp{ 0.0f, 1.0f, 0.0f };
        constexpr glm::vec3 TargetPosition{ 0.0f };
    }

    bool is_close(float a, float b, float tolerance = 1e-5f)
    {
        return std::fabs(a - b) <= tolerance;
    }

    void override_if_close(float& target, float number, float tolerance = 1e-2f)
    {
        if (is_close(target, number, tolerance))
        {
            target = number;
        }
    }

    void override_if_close(glm::vec4& target, float number)
    {
        override_if_close(target.x, number);
        override_if_close(target.y, number);
        override_if_close(target.z, number);
        override_if_close(target.w, number);
    }

    void override_if_close(glm::vec2& target, float number)
    {
        override_if_close(target.x, number);
        override_if_close(target.y, number);
    }
}

namespace demos
{
    D07TessellationShaders::D07TessellationShaders()
    {
        GL::ClearColor(0.392f, 0.584f, 0.929f, 1.0f);

        {
            using namespace asset_paths;

            const struct
            {
                gsl::czstring                        ShaderName;
                std::array<std::filesystem::path, 5> ShaderFilePaths;
            } shader_setup[Materials::Count] = {
                {QuadSpacingEqualShaderName, { ParamsVertexPath, ParamsQuadControlPath, ParamsQuadSpacingEqualEvalPath, WireframeGeometryPath, Fill3DFragmentPath }},
                { QuadSpacingEvenShaderName,  { ParamsVertexPath, ParamsQuadControlPath, ParamsQuadSpacingEvenEvalPath, WireframeGeometryPath, Fill3DFragmentPath }},
                {  QuadSpacingOddShaderName,   { ParamsVertexPath, ParamsQuadControlPath, ParamsQuadSpacingOddEvalPath, WireframeGeometryPath, Fill3DFragmentPath }},
                { TriSpacingEqualShaderName,   { ParamsVertexPath, ParamsTriControlPath, ParamsTriSpacingEqualEvalPath, WireframeGeometryPath, Fill3DFragmentPath }},
                {  TriSpacingEvenShaderName,    { ParamsVertexPath, ParamsTriControlPath, ParamsTriSpacingEvenEvalPath, WireframeGeometryPath, Fill3DFragmentPath }},
                {   TriSpacingOddShaderName,     { ParamsVertexPath, ParamsTriControlPath, ParamsTriSpacingOddEvalPath, WireframeGeometryPath, Fill3DFragmentPath }},
                {LineSpacingEqualShaderName,    { ParamsVertexPath, ParamsLineControlPath, ParamsLineSpacingEqualEvalPath, PointsGeometryPath, Fill3DFragmentPath }},
                { LineSpacingEvenShaderName,     { ParamsVertexPath, ParamsLineControlPath, ParamsLineSpacingEvenEvalPath, PointsGeometryPath, Fill3DFragmentPath }},
                {  LineSpacingOddShaderName,      { ParamsVertexPath, ParamsLineControlPath, ParamsLineSpacingOddEvalPath, PointsGeometryPath, Fill3DFragmentPath }},
            };

            for (size_t i = 0; i < Materials::Count; ++i)
            {
                const auto& setup = shader_setup[i];
                const auto  paths = std::span{ setup.ShaderFilePaths };
                assetReloader.SetAndAutoReloadShader(shaders[i], setup.ShaderName, paths);
                materials[i]                 = graphics::Material(&shaders[i], setup.ShaderName + " Material"s);
                materials[i].Culling.Enabled = false;
            }
        }

        for (auto& material : materials)
        {
            material.SetMaterialUniform(Uniforms::DiffuseMaterial, glm::vec3(0.19f, 0.191f, 0.191f));
        }

        buildMeshes();

        const float aspect = static_cast<float>(environment::DisplayWidth) / static_cast<float>(environment::DisplayHeight);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);
        ViewMatrix         = glm::lookAt(camera::EyePosition, camera::TargetPosition, camera::RelativeUp);
    }

    void D07TessellationShaders::Update()
    {
        assetReloader.Update();

        constexpr double FUDGE_FACTOR = 2.5;
        const auto       easing       = std::min(static_cast<float>(environment::DeltaTime * FUDGE_FACTOR), 1.0f);
        const auto       shape_index  = static_cast<size_t>(shape);
        outsideTessellation[shape_index] += easing * (targetOutsideTessellation - outsideTessellation[shape_index]);
        insideTessellation[shape_index] += easing * (targetInsideTessellation - insideTessellation[shape_index]);

        override_if_close(outsideTessellation[shape_index], 1.0f);
        override_if_close(insideTessellation[shape_index], 1.0f);
        override_if_close(outsideTessellation[shape_index], 64.0f);
        override_if_close(insideTessellation[shape_index], 64.0f);

        for (auto& material : materials)
        {
            material.SetMaterialUniform(Uniforms::Projection, ProjectionMatrix);
            material.SetMaterialUniform(Uniforms::ViewMatrix, ViewMatrix);
            material.SetMaterialUniform(Uniforms::ModelMatrix, glm::mat4(1.0f));
            material.SetMaterialUniform(Uniforms::OutsideTessellation, outsideTessellation[shape_index]);
            material.SetMaterialUniform(Uniforms::InsideTessellation, insideTessellation[shape_index]);
        }
    }

    void D07TessellationShaders::Draw() const
    {
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL::Enable(GL_PROGRAM_POINT_SIZE);
        const auto material_index = static_cast<Materials::Type>(int(shape) * int(Tessellation::SpacingCount) + int(spacing));
        materials[material_index].ForceApplyAllSettings();
#if !defined(OPENGL_ES3_ONLY)
        {
            /* TODO
             * if drawing the triangles shape use patch size of 3 otherwise use size of 4
             * GL::PatchParameteri - https://docs.gl/gl4/glPatchParameter
             */
            if (shape == Tessellation::Shape::triangles)
            {
                GL::PatchParameteri(GL_PATCH_VERTICES, 3); // 삼각형 패치에는 3개의 정점 필요
            }
            else if (shape == Tessellation::Shape::isolines)
            {
                GL::PatchParameteri(GL_PATCH_VERTICES, 2); // isolines에는 2개의 정점 필요
            }
            else
            {
                GL::PatchParameteri(GL_PATCH_VERTICES, 4); // 나머지(quads)는 4개의 정점
            }

        }
#endif
        const auto shape_index = static_cast<size_t>(shape);
        meshes[shape_index].Use();
        GLDrawIndexed(meshes[shape_index]);
    }

    void D07TessellationShaders::ImGuiDraw()
    {
        constexpr auto shape_names = "quads\0triangles\0isolines\0\0";
        ImGui::Combo("Shape", reinterpret_cast<int*>(&shape), shape_names);
        constexpr auto spacing_names = "equal_spacing\0fractional_even_spacing\0fractional_odd_spacing\0\0";
        ImGui::Combo("Spacing", reinterpret_cast<int*>(&spacing), spacing_names);
        ImGui::SliderFloat4("Outside Tessellation", &targetOutsideTessellation.x, 1.0f, 64.0f);
        ImGui::SliderFloat2("Inside Tessellation", &targetInsideTessellation.x, 1.0f, 64.0f);
    }

    void D07TessellationShaders::SetDisplaySize(int width, int height)
    {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        ProjectionMatrix   = glm::perspective(camera::FOV, aspect, camera::NearDistance, camera::FarDistance);
    }

    void D07TessellationShaders::buildMeshes()
    {
        /* TODO
         * Create a quad mesh with positions  { -1, 0, -1 }, { -1, 0, 1 }, { 1, 0, -1 }, { 1, 0, 1 }
         *                      index buffer of {0, 1, 2, 3}
         *                      prim pattern of Patches
         * Save into meshes[Tessellation::Shape::quads]
         */

        /* TODO
         * Create a triangle mesh with positions  { -1, 0, -1 }, { -1, 0, 1 }, { 1, 0, -1 }
         *                      index buffer of {0, 1, 2}
         *                      prim pattern of Patches
         * Save into meshes[Tessellation::Shape::triangles]
         */

        /* TODO
         * Create an isolines mesh with positions  { -1, 0, -1 }, { -1, 0, 1 }, { 1, 0, -1 }, { 1, 0, 1 }
         *                      index buffer of {0, 1, 2, 3}
         *                      prim pattern of Patches
         * Save into meshes[Tessellation::Shape::isolines]
         */

        // quad mesh
        {
            const std::array quadVertices = 
            {
                glm::vec3{-1.0f, 0.0f, -1.0f}, // bottom left
                glm::vec3{-1.0f, 0.0f,  1.0f}, // top left
                glm::vec3{ 1.0f, 0.0f, -1.0f}, // bottom right
                glm::vec3{ 1.0f, 0.0f,  1.0f}  // top right
            };
            const std::array quadIndices = { 0u, 1u, 2u, 3u };

            GLVertexBuffer quadVertexBuffer(std::span{ quadVertices });
            GLIndexBuffer  quadIndexBuffer(quadIndices);

            GLAttributeLayout positionLayout;
            positionLayout.component_type         = GLAttributeLayout::Float;
            positionLayout.component_dimension    = GLAttributeLayout::_3;
            positionLayout.normalized             = false;
            positionLayout.vertex_layout_location = 0;
            positionLayout.stride                 = sizeof(glm::vec3);
            positionLayout.offset                 = 0;
            positionLayout.relative_offset        = 0;

            meshes[Tessellation::Shape::quads].AddVertexBuffer(std::move(quadVertexBuffer), { positionLayout });
            meshes[Tessellation::Shape::quads].SetIndexBuffer(std::move(quadIndexBuffer));
            meshes[Tessellation::Shape::quads].SetPrimitivePattern(GLPrimitive::Patches);
        }

        // triangle mesh
        {
            const std::array triVertices = 
            {
				glm::vec3{-1.0f, 0.0f, -1.0f}, // bottom left
				glm::vec3{-1.0f, 0.0f,  1.0f}, // top left
				glm::vec3{ 1.0f, 0.0f, -1.0f}  // bottom right
			};
			const std::array triIndices = { 0u, 1u, 2u };

			GLVertexBuffer triVertexBuffer(std::span{ triVertices });
			GLIndexBuffer  triIndexBuffer(triIndices);

			GLAttributeLayout positionLayout;
			positionLayout.component_type         = GLAttributeLayout::Float;
			positionLayout.component_dimension    = GLAttributeLayout::_3;
			positionLayout.normalized             = false;
			positionLayout.vertex_layout_location = 0;
			positionLayout.stride                 = sizeof(glm::vec3);
			positionLayout.offset                 = 0;
			positionLayout.relative_offset        = 0;

			meshes[Tessellation::Shape::triangles].AddVertexBuffer(std::move(triVertexBuffer), { positionLayout });
			meshes[Tessellation::Shape::triangles].SetIndexBuffer(std::move(triIndexBuffer));
			meshes[Tessellation::Shape::triangles].SetPrimitivePattern(GLPrimitive::Patches);
		}

        // isolines mesh
        {
            const std::array isolineVertices = 
            {
                glm::vec3{-1.0f, 0.0f, -1.0f}, // point 1
                glm::vec3{-1.0f, 0.0f,  1.0f}, // point 2
                glm::vec3{ 1.0f, 0.0f, -1.0f}, // point 3
                glm::vec3{ 1.0f, 0.0f,  1.0f}  // point 4
            };
            const std::array isolineIndices = { 0u, 1u, 2u, 3u };

            GLVertexBuffer isolineVertexBuffer(std::span{ isolineVertices });
            GLIndexBuffer  isolineIndexBuffer(isolineIndices);

            GLAttributeLayout positionLayout;
            positionLayout.component_type         = GLAttributeLayout::Float;
            positionLayout.component_dimension    = GLAttributeLayout::_3;
            positionLayout.normalized             = false;
            positionLayout.vertex_layout_location = 0;
            positionLayout.stride                 = sizeof(glm::vec3);
            positionLayout.offset                 = 0;
            positionLayout.relative_offset        = 0;

            meshes[Tessellation::Shape::isolines].AddVertexBuffer(std::move(isolineVertexBuffer), { positionLayout });
            meshes[Tessellation::Shape::isolines].SetIndexBuffer(std::move(isolineIndexBuffer));
            meshes[Tessellation::Shape::isolines].SetPrimitivePattern(GLPrimitive::Patches);
        }
    }
}
