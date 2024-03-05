/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
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
    /* TODO
        if can do opengl 4.5
            GL::CreateBuffers - https://docs.gl/gl4/glCreateBuffers
            GL::NamedBufferStorage - https://docs.gl/gl4/glBufferStorage
        else
            GL::GenBuffers - https://docs.gl/es3/glGenBuffers
            GL::BindBuffer - https://docs.gl/es3/glBindBuffer
            GL::BufferData - https://docs.gl/es3/glBufferData
            GL::BindBuffer - (unbind)
    */ 
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
    // TODO GL::DeleteBuffers - https://docs.gl/es3/glDeleteBuffers
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
    // TODO GL::BindBuffer - https://docs.gl/es3/glBindBuffer
    GL::BindBuffer(GL_ARRAY_BUFFER, bind ? buffer_handle : 0);
}

void GLVertexBuffer::send_buffer_data_to_gpu(const void* data, GLsizei size_bytes, GLsizei starting_offset) const
{
    /* TODO
        if can do opengl 4.5
            GL::NamedBufferSubData - https://docs.gl/gl4/glBufferSubData
        else
            GL::BindBuffer - https://docs.gl/es3/glBindBuffer
            GL::BufferSubData - https://docs.gl/es3/glBufferSubData
            GL::BindBuffer - unbind
    */
    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::NamedBufferStorage(buffer_handle, size, data, GL_DYNAMIC_STORAGE_BIT);
    }
    else
    {

    }
}
