/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

namespace environment::opengl
{
    inline int  MajorVersion         = 0;
    inline int  MinorVersion         = 0;
    inline bool IsOpenGL_ES          = false;
    inline int  MaxElementVertices   = 0;
    inline int  MaxElementIndices    = 0;
    inline int  MaxTextureImageUnits = 2;
    inline int  MaxTextureSize       = 64;

    constexpr int version(int major, int minor) noexcept
    {
        return major * 100 + minor * 10;
    }

    inline int current_version() noexcept
    {
        return version(MajorVersion, MinorVersion);
    }
}

#if !defined(OPENGL_ES3_ONLY)
#    define IF_CAN_DO_OPENGL(major, minor) if (environment::opengl::current_version() >= environment::opengl::version(major, minor))
#else
#    define IF_CAN_DO_OPENGL(major, minor) if constexpr (environment::opengl::version(major, minor) <= environment::opengl::version(3, 0))
#endif
