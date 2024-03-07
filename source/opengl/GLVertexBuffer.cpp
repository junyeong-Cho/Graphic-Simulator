/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "GLVertexBuffer.hpp"

#include "GL.hpp"
#include "environment/OpenGL.hpp"
#include <GL/glew.h>

GLVertexBuffer::GLVertexBuffer(GLsizei size_in_bytes)
    : size(size_in_bytes)
{
    constexpr const void* no_data = nullptr;
   
    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::CreateBuffers(1, &buffer_handle);
        GL::NamedBufferStorage(buffer_handle, size, no_data, GL_DYNAMIC_STORAGE_BIT);
    }
    else
    {
        GL::GenBuffers(1, &buffer_handle);
        GL::BindBuffer(GL_ARRAY_BUFFER, buffer_handle);
        GL::BufferData(GL_ARRAY_BUFFER, size, no_data, GL_DYNAMIC_DRAW);
        GL::BindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

GLVertexBuffer::~GLVertexBuffer()
{
    GL::DeleteBuffers(1, &buffer_handle);
}

GLVertexBuffer::GLVertexBuffer(GLVertexBuffer&& temp) noexcept
    : size(temp.size), buffer_handle(temp.buffer_handle)
{
    temp.buffer_handle = 0;
    temp.size          = 0;
}

GLVertexBuffer& GLVertexBuffer::operator=(GLVertexBuffer&& temp) noexcept
{
    std::swap(buffer_handle, temp.buffer_handle);
    std::swap(size, temp.size);

    return *this;
}

void GLVertexBuffer::Use(bool bind) const
{
    GL::BindBuffer(GL_ARRAY_BUFFER, bind ? buffer_handle : 0);
}

void GLVertexBuffer::send_buffer_data_to_gpu(const void* data, GLsizei size_bytes, GLsizei starting_offset) const
{

    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::NamedBufferSubData(buffer_handle, starting_offset, size_bytes, data); 
    }
    else
    {
        GL::BindBuffer(GL_ARRAY_BUFFER, buffer_handle);
        GL::BufferSubData(GL_ARRAY_BUFFER, starting_offset, size_bytes, data);
        GL::BindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
