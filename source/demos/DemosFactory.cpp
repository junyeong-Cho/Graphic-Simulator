/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "DemosFactory.hpp"

#include "D01HelloQuad.hpp"


#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace demos
{

    gsl::owner<IDemo*> create_demo(Demos the_demo)
    {
        switch (the_demo)
        {
            case Demos::None:
            case Demos::HelloQuad: return new D01HelloQuad();
            default: throw std::runtime_error{ "Tried to create a demo we don't have yet...\n" }; break;
        }
    }

    Demos string_to_demo(std::string_view str) noexcept
    {
        static const std::unordered_map<std::string_view, Demos> demo_map{
            { "hello",        Demos::HelloQuad}
        };
        const auto to_lower = [](std::string_view s)
        {
            std::string r(s);
            std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c)
                           { return static_cast<char>(std::tolower(c)); });
            return r;
        };
        std::string lowercase_str = to_lower(str);
        auto        it            = demo_map.find(lowercase_str);
        if (it != demo_map.end())
        {
            return it->second;
        }
        else
        {
            return Demos::HelloQuad;
        }
    }
}
