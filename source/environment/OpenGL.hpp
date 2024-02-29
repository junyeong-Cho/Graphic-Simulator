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
}

#if !defined(OPENGL_ES3_ONLY)
#    define IF_CAN_DO_OPENGL(major, minor) if (environment::opengl::MajorVersion * 100 + environment::opengl::MinorVersion * 10 >= major * 100 + minor * 10)
#else
#    define IF_CAN_DO_OPENGL(major, minor) if constexpr (300 >= major * 100 + minor * 10)
#endif
