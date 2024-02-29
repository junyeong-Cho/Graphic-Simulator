/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "IDemo.hpp"
#include <gsl/pointers>
#include <string_view>

namespace demos
{
    enum class Demos
    {
        None,
        HelloQuad,
    };

    gsl::owner<IDemo*> create_demo(Demos the_demo);

    [[nodiscard]] Demos string_to_demo(std::string_view str) noexcept;
}
