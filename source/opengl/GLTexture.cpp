/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
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

GLTexture::GLTexture(GLTexture&& other) noexcept
    : texture_handle(other.texture_handle), width(other.width), height(other.height)
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
    RGBA* const rgba_pixels =
        reinterpret_cast<RGBA*>(stbi_load(image_filepath.string().c_str(), &pixel_width, &pixel_height, &files_channels_count, desired_channel_count));
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
    /* TODO
    if can do opengl 4.5
        GL::CreateTextures - https://docs.gl/gl4/glCreateTextures
        GL::TextureStorage2D  - GL_RGBA8 https://docs.gl/gl4/glTexStorage2D
        GL::TextureSubImage2D - GL_RGBA GL_UNSIGNED_BYTE https://docs.gl/gl4/glTexSubImage2D
        GL::TextureParameteri - set min filter https://docs.gl/gl4/glTexParameter
        GL::TextureParameteri - set mag filter
        GL::TextureParameteri - set wrap s 
        GL::TextureParameteri - set wrap t
    else
        GL::GenTextures - https://docs.gl/es3/glGenTextures
        GL::BindTexture - https://docs.gl/es3/glBindTexture
        GL::TexParameteri - set min filter - https://docs.gl/es3/glTexParameter
        GL::TexParameteri - set mag filter
        GL::TexParameteri - set wrap s 
        GL::TexParameteri - set wrap t
        GL::TexImage2D - https://docs.gl/es3/glTexImage2D
        GL::BindTexture - unbind
    */
    return true;
}

void GLTexture::UseForSlot(unsigned texture_unit) const noexcept
{
    /* TODO
    if can do opengl 4.5
        GL::BindTextureUnit - https://docs.gl/gl4/glBindTextureUnit
    else
        GL::ActiveTexture - https://docs.gl/es3/glActiveTexture
        GL::BindTexture - https://docs.gl/es3/glBindTexture
    */
}

void GLTexture::SetFiltering(Filtering how_to_filter) noexcept
{
    if (filtering == how_to_filter || texture_handle == 0)
        return;

    filtering = how_to_filter;
    /* TODO
    if can do opengl 4.5
        GL::TextureParameteri - set min filter https://docs.gl/gl4/glTexParameter
        GL::TextureParameteri -  set max filter
    else
        GL::BindTexture
        GL::TexParameteri - set min filter https://docs.gl/es3/glTexParameter
        GL::TexParameteri -  set max filter
    */
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

    /* TODO
    if can do opengl 4.5
        for each coordinate
            GL::TextureParameteri - https://docs.gl/gl4/glTexParameter
    else
        GL::BindTexture - https://docs.gl/es3/glBindTexture
        for each coordinate
            GL::TexParameteri - https://docs.gl/es3/glTexParameter

    */
}

void GLTexture::delete_texture() noexcept
{
    // TODO GL::DeleteTextures - https://docs.gl/es3/glDeleteTextures
    texture_handle = 0;
    width          = 0;
    height         = 0;
}
