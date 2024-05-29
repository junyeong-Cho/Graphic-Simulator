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
        const auto Projection  = "uProjection"s;
        const auto TileScale   = "uTileScale"s;
        const auto Z           = "uZ"s;
        const auto Pattern     = "uPattern"s;
        const auto ModelMatrix = "uModelMatrix"s;
        const auto ViewMatrix  = "uViewMatrix"s;

    }

    namespace asset_paths
    {
        constexpr auto SimpleVertexPath   = "D10GradientNoise/simple.vert";
        constexpr auto SimpleFragmentPath = "D10GradientNoise/simple.frag";

        constexpr auto GenGradientNoiseVertexPath   = "D10GradientNoise/gen_gradient_noise.vert";
        constexpr auto GenGradientNoiseFragmentPath = "D10GradientNoise/gen_gradient_noise.frag";

        constexpr auto SurfaceVertexPath   = "D10GradientNoise/surface.vert";
        constexpr auto SurfaceFragmentPath = "D10GradientNoise/simple.frag";
    }

    namespace camera
    {
        constexpr float FOV          = glm::radians(60.0f);
        constexpr float NearDistance = 0.05f;
        constexpr float FarDistance  = 100.0f;
    }

    template <typename T, typename... Values>
    [[nodiscard]] constexpr bool contains(const std::span<T>& container, Values... values) noexcept
    {
        auto contains_value = [&values...](const T& element)
        {
            return ((element == values) || ...);
        };
        const auto the_end = std::end(container);
        auto       result  = std::find_if(std::begin(container), the_end, contains_value);
        return result != the_end;
    }
}

namespace demos
{
    D11CurvesNSplines::D11CurvesNSplines()
    {
         
    }

    void D11CurvesNSplines::Update()
    {

    }

    void D11CurvesNSplines::Draw() const
    {

    }

    void D11CurvesNSplines::ImGuiDraw()
    {

    }

    void D11CurvesNSplines::SetDisplaySize(int width, int height)
    {

    }
}
