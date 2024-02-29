/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "Path.hpp"

#include <SDL.h>
#include <optional>

namespace
{
    std::optional<std::filesystem::path> try_get_asset_path(const std::filesystem::path& starting_directory)
    {
        namespace fs        = std::filesystem;
        fs::path       p    = starting_directory;
        const fs::path root = p.root_path();
        do
        {
            fs::path assets_folder = fs::absolute(p / "assets");
            if (fs::is_directory(assets_folder))
            {
                return std::optional<fs::path>{ assets_folder };
            }
            p = fs::absolute(p / "..");
        } while (fs::absolute(p) != root);
        return std::optional<fs::path>{};
    }
}

namespace assets
{

    std::filesystem::path get_base_path()
    {
        namespace fs                  = std::filesystem;
        static fs::path assets_folder = []()
        {
            auto result = try_get_asset_path(fs::current_path());
            if (result)
                return result.value();
            // try from the exe path rather than the current working directory
            const auto base_path = SDL_GetBasePath();
            result               = try_get_asset_path(base_path);
            SDL_free(base_path);
            if (result)
                return result.value();
            throw std::runtime_error{ "Failed to find assets folder in parent folders" };
        }();
        return assets_folder;
    }
}
