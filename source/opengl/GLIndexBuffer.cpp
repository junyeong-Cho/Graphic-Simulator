/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
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
        IF_CAN_DO_OPENGL(4, 5)
        {
			unsigned buffer_handle;
			GL::CreateBuffers(1, &buffer_handle);
			GL::NamedBufferStorage(buffer_handle, size_bytes, data, 0);
			return buffer_handle;
		}
        else
        {
			unsigned buffer_handle;
			GL::GenBuffers(1, &buffer_handle);
			GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_handle);
			GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, size_bytes, data, GL_STATIC_DRAW);
			GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			return buffer_handle;
		}

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
    GL::DeleteBuffers(1, &indices_handle);
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
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, bind ? indices_handle : 0);
}
