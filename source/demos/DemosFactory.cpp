/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "DemosFactory.hpp"

#include "D01HelloQuad.hpp"
#include "D02ProceduralMeshes.hpp"
#include "D03Fog.hpp"
#include "D04ToonShading.hpp"
#include "D05ShadowMapping.hpp"
#include "D06GeometryShaders.hpp"


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
            case Demos::HelloQuad:        return new D01HelloQuad();
            case Demos::ProceduralMeshes: return new D02ProceduralMeshes();
            case Demos::Fog:              return new D03Fog();
            case Demos::ToonShading:      return new D04ToonShading();
            case Demos::ShadowMapping:    return new D05ShadowMapping();    
            case Demos::GeometryShaders:  return new D06GeometryShaders();
            default: throw std::runtime_error{ "Tried to create a demo we don't have yet...\n" }; break;
        }
    }

    Demos string_to_demo(std::string_view str) noexcept
    {
        static const std::unordered_map<std::string_view, Demos> demo_map{
            { "hello",        Demos::HelloQuad},
            {"meshes",        Demos::ProceduralMeshes},
            {"fog",           Demos::Fog},
			{"toon",          Demos::ToonShading},
			{"shadow",        Demos::ShadowMapping},
            {"geom",          Demos::GeometryShaders},
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
