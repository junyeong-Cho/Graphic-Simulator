/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Material.hpp"

#include <numbers>
#include <opengl/GLVertexArray.hpp>

namespace graphics
{

    constexpr float PI = std::numbers::pi_v<float>;

    struct MeshVertex
    {
        constexpr MeshVertex() noexcept = default;

        constexpr MeshVertex(glm::vec3 the_position, glm::vec3 the_normal, glm::vec2 the_uv) noexcept
            : position(the_position), normal(the_normal), uv(the_uv)
        {
        }

        glm::vec3 position{}, normal{};
        glm::vec2 uv{};
    };

    void describe_meshvertex_layout(GLAttributeLayout& position, GLAttributeLayout& normal, GLAttributeLayout& uv);

    struct SubMesh
    {
        GLVertexArray       VertexArrayObj{};
        graphics::Material* Material{ nullptr };
    };

    struct Mesh
    {
        std::string          Name = "unnamed mesh";
        std::vector<SubMesh> SubMeshes{};
    };

    struct Geometry
    {
        std::vector<MeshVertex> Vertices{};
        std::vector<unsigned>   Indicies{};
    };

    Geometry create_plane(int stacks, int slices);
    Geometry create_cube(int stacks, int slices);
    Geometry create_line(const std::vector<glm::vec3>& points);
    Geometry create_sphere(int stacks, int slices);
    Geometry create_torus(int stacks, int slices, float start_angle = 0, float end_angle = 2.0f * PI);
    Geometry create_cylinder(int stacks, int slices);
    Geometry create_cone(int stacks, int slices);
    Geometry create_trefoil(int stacks, int slices);
    SubMesh  to_submesh_as_triangles(const Geometry& geometry, Material* material = nullptr);
    SubMesh  to_submesh_as_lines(const Geometry& geometry, Material* material = nullptr);
}
