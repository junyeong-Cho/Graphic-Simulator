/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "GLIndexBuffer.hpp"

#include "GL.hpp"
#include "environment/OpenGL.hpp"
#include <GL/glew.h>

namespace
{
    unsigned create_indices_buffer(const void* data, GLsizei size_bytes)
    {
        /*
        TODO
        if can do opengl 4.5
            GL::CreateBuffers      - https://docs.gl/gl4/glCreateBuffers
            GL::NamedBufferStorage - https://docs.gl/gl4/glBufferStorage
        else
             GL::GenBuffers - https://docs.gl/es3/glGenBuffers
             GL::BindBuffer - bind newly created buffer https://docs.gl/es3/glBindBuffer
             GL::BufferData - https://docs.gl/es3/glBufferData
             GL::BindBuffer - unbind newly created buffer
        */
    }
}

GLIndexBuffer::GLIndexBuffer(std::span<const unsigned> indices)
    : element_type(GLIndexElement::UInt), count(static_cast<GLsizei>(indices.size()))
{
    indices_handle = create_indices_buffer(indices.data(), static_cast<GLsizei>(indices.size_bytes()));
}

GLIndexBuffer::GLIndexBuffer(std::span<const unsigned short> indices)
    : element_type(GLIndexElement::UShort), count(static_cast<GLsizei>(indices.size()))
{
    indices_handle = create_indices_buffer(indices.data(), static_cast<GLsizei>(indices.size_bytes()));
}

GLIndexBuffer::GLIndexBuffer(std::span<const unsigned char> indices)
    : element_type(GLIndexElement::UByte), count(static_cast<GLsizei>(indices.size()))
{
    indices_handle = create_indices_buffer(indices.data(), static_cast<GLsizei>(indices.size_bytes()));
}

GLIndexBuffer::~GLIndexBuffer()
{
    // TODO use GL::DeleteBuffers - https://docs.gl/es3/glDeleteBuffers
}

GLIndexBuffer::GLIndexBuffer(GLIndexBuffer&& temp) noexcept
    : element_type(temp.element_type), count(temp.count), indices_handle(temp.indices_handle)
{
    temp.element_type   = GLIndexElement::None;
    temp.count          = 0;
    temp.indices_handle = 0;
}

GLIndexBuffer& GLIndexBuffer::operator=(GLIndexBuffer&& temp) noexcept
{
    std::swap(element_type, temp.element_type);
    std::swap(count, temp.count);
    std::swap(indices_handle, temp.indices_handle);

    return *this;
}

void GLIndexBuffer::Use(bool bind) const
{
    // TODO use GL::BindBuffer - https://docs.gl/es3/glBindBuffer
}
