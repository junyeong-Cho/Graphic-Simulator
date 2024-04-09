/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "GLTexture.hpp"

#include "GL.hpp"
#include "assets/Path.hpp"
#include "environment/OpenGL.hpp"
#include <GL/glew.h>
#include <stb_image.h>

GLTexture::~GLTexture() noexcept
{
    delete_texture();
}

GLTexture::GLTexture(GLTexture&& other) noexcept : texture_handle(other.texture_handle), width(other.width), height(other.height)
{
    other.texture_handle = 0;
    other.width          = 0;
    other.height         = 0;
}

GLTexture& GLTexture::operator=(GLTexture&& other) noexcept
{
    std::swap(texture_handle, other.texture_handle);
    std::swap(width, other.width);
    std::swap(height, other.height);
    return *this;
}

bool GLTexture::LoadAsDepthTexture(int image_width, int image_height, DepthComponentSize bit_depth) noexcept
{
    delete_texture();
    width  = image_width;
    height = image_height;


    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::CreateTextures(GL_TEXTURE_2D, 1, &texture_handle);
        // Use bit_depth as the format for the depth component
        GL::TextureStorage2D(texture_handle, 1, static_cast<GLenum>(bit_depth), width, height);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    }
    else
    {
        GL::GenTextures(1, &texture_handle);
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);

        if (!environment::opengl::IsOpenGL_ES)
        {
            IF_CAN_DO_OPENGL(4, 2)
            {
                GLenum format = GL_DEPTH_COMPONENT;
                switch (bit_depth)
                {
                    case DepthBits16: format = GL_DEPTH_COMPONENT16; break;
                    case DepthBits24: format = GL_DEPTH_COMPONENT24; break;
                    case DepthBits32: format = GL_DEPTH_COMPONENT32; break;
                    case DepthBits32F: format = GL_DEPTH_COMPONENT32F; break;
                    default: break;
                }
                GL::TexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            }
        }
        else
        {
            GL::TexStorage2D(GL_TEXTURE_2D, 1, static_cast<GLenum>(bit_depth), width, height);
        }

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    return true;
}

bool GLTexture::LoadAsRGBA(int image_width, int image_height) noexcept
{
    return LoadFromMemory(image_width, image_height, nullptr);
}

bool GLTexture::LoadFromFileImage(std::filesystem::path image_filepath, bool flip_vertical) noexcept
{
    if (!std::filesystem::exists(image_filepath))
    {
        // try prepending the asset directory path
        image_filepath = assets::get_base_path() / image_filepath;
        if (!std::filesystem::exists(image_filepath))
        {
            return false;
        }
    }
    int           pixel_width;
    int           pixel_height;
    int           files_channels_count  = 0;
    constexpr int desired_channel_count = 4;
    stbi_set_flip_vertically_on_load(flip_vertical);
    RGBA* const rgba_pixels = reinterpret_cast<RGBA*>(stbi_load(image_filepath.string().c_str(), &pixel_width, &pixel_height, &files_channels_count, desired_channel_count));
    if (rgba_pixels == nullptr)
        return false;
    const bool result = LoadFromMemory(pixel_width, pixel_height, rgba_pixels);
    stbi_image_free(rgba_pixels);
    return result;
}

bool GLTexture::LoadFromMemory(int image_width, int image_height, const RGBA* colors) noexcept
{
    delete_texture();
    width  = image_width;
    height = image_height;

    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::CreateTextures(GL_TEXTURE_2D, 1, &texture_handle);
        GL::TextureStorage2D(texture_handle, 1, GL_RGBA8, width, height);
        if (colors != nullptr)
        {
            GL::TextureSubImage2D(texture_handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colors);
        }

        GL::TextureParameteri(texture_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        GL::GenTextures(1, &texture_handle);
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);

        GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colors);

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    return true;
}

void GLTexture::UseForSlot(unsigned texture_unit) const noexcept
{
    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::BindTextureUnit(texture_unit, texture_handle);
    }
    else
    {
        GL::ActiveTexture(GL_TEXTURE0 + texture_unit);
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
    }
}

void GLTexture::SetFiltering(Filtering how_to_filter) noexcept
{
    if (filtering == how_to_filter || texture_handle == 0)
        return;

    filtering = how_to_filter;

    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::TextureParameteri(texture_handle, GL_TEXTURE_MIN_FILTER, (filtering == Filtering::NearestPixel) ? GL_NEAREST : GL_LINEAR);
        GL::TextureParameteri(texture_handle, GL_TEXTURE_MAG_FILTER, (filtering == Filtering::NearestPixel) ? GL_NEAREST : GL_LINEAR);
    }
    else
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (filtering == Filtering::NearestPixel) ? GL_NEAREST : GL_LINEAR);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filtering == Filtering::NearestPixel) ? GL_NEAREST : GL_LINEAR);
    }
}

void GLTexture::SetWrapping(Wrapping how_to_wrap, Coordinate coord) noexcept
{
    if (texture_handle == 0)
        return;

    GLenum coords_to_set[2] = {};
    int    num_coords       = 1;
    switch (coord)
    {
        case Coordinate::S:
            coords_to_set[0] = GL_TEXTURE_WRAP_S;
            wrapping[S]      = how_to_wrap;
            break;
        case Coordinate::T:
            coords_to_set[0] = GL_TEXTURE_WRAP_T;
            wrapping[T]      = how_to_wrap;
            break;
        case Coordinate::Both:
            coords_to_set[0] = GL_TEXTURE_WRAP_S;
            coords_to_set[1] = GL_TEXTURE_WRAP_T;
            wrapping[S]      = how_to_wrap;
            wrapping[T]      = how_to_wrap;
            num_coords       = 2;
            break;
    }


    GLenum gl_wrap_mode = (how_to_wrap == Wrapping::Repeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE;

    IF_CAN_DO_OPENGL(4, 5)
    {
        for (int i = 0; i < num_coords; i++)
        {
            GL::TextureParameteri(texture_handle, coords_to_set[i], static_cast<int>(gl_wrap_mode));
        }
    }
    else
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        for (int i = 0; i < num_coords; i++)
        {
            GL::TexParameteri(GL_TEXTURE_2D, coords_to_set[i], static_cast<int>(gl_wrap_mode));
        }
    }
}

void GLTexture::delete_texture() noexcept
{
    GL::DeleteTextures(1, &texture_handle);
    texture_handle = 0;
    width          = 0;
    height         = 0;
}
