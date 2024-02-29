/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "GLVertexArray.hpp"

#include "GL.hpp"
#include "environment/OpenGL.hpp"
#include <GL/glew.h>
#include <gsl/gsl>

GLVertexArray::GLVertexArray(GLPrimitive::Type the_primitive_pattern)
{
    primitive_pattern = the_primitive_pattern;
    /* TODO
    if can do opengl 4.5
        GL::CreateVertexArrays - https://docs.gl/gl4/glCreateVertexArrays
    else
        GL::GenVertexArrays - https://docs.gl/es3/glGenVertexArrays
    */
}

GLVertexArray::~GLVertexArray()
{
    // TODO GL::DeleteVertexArrays - https://docs.gl/es3/glDeleteVertexArrays
}

GLVertexArray::GLVertexArray(GLVertexArray&& temp) noexcept
    : vertex_array_handle(temp.vertex_array_handle), vertex_buffers(std::move(temp.vertex_buffers)), index_buffer(std::move(temp.index_buffer)),
      num_indices(temp.num_indices), indices_type(temp.indices_type), primitive_pattern(temp.primitive_pattern), num_vertices(temp.num_vertices)
{
    temp.vertex_array_handle = 0;
    temp.num_indices         = 0;
    temp.indices_type        = GLIndexElement::None;
    temp.num_vertices        = 0;
}

GLVertexArray& GLVertexArray::operator=(GLVertexArray&& temp) noexcept
{
    std::swap(vertex_array_handle, temp.vertex_array_handle);
    std::swap(vertex_buffers, temp.vertex_buffers);
    std::swap(index_buffer, temp.index_buffer);
    std::swap(num_indices, temp.num_indices);
    std::swap(indices_type, temp.indices_type);
    std::swap(primitive_pattern, temp.primitive_pattern);
    std::swap(num_vertices, temp.num_vertices);

    return *this;
}

void GLVertexArray::Use(bool bind) const
{
    // TODO GL::BindVertexArray - https://docs.gl/es3/glBindVertexArray
}

void GLVertexArray::AddVertexBuffer(GLVertexBuffer&& vertex_buffer, std::initializer_list<GLAttributeLayout> buffer_layout)
{
    const auto buffer_handle = vertex_buffer.GetHandle();
    for (const auto& attribute : buffer_layout)
    {
        /* TODO
        if can do opengl 4.5
            GL::EnableVertexArrayAttrib - https://docs.gl/gl4/glEnableVertexAttribArray
            GL::VertexArrayVertexBuffer - https://docs.gl/gl4/glBindVertexBuffer
            GL::VertexArrayAttribFormat - https://docs.gl/gl4/glVertexAttribFormat
            GL::VertexArrayAttribBinding - https://docs.gl/gl4/glVertexAttribBinding
        else
            Use (bind) this Vertex Array
            Use (bind) the Vertex Buffer
            GL::EnableVertexAttribArray - https://docs.gl/es3/glEnableVertexAttribArray
            GL::VertexAttribPointer - https://docs.gl/es3/glGetVertexAttribPointerv
            Do not Use (unbind) this vertex array
            Do not Use (unbind) the vertex buffer

        */
    }
    vertex_buffers.emplace_back(std::move(vertex_buffer));
}

void GLVertexArray::SetIndexBuffer(GLIndexBuffer&& the_indices)
{
    num_indices  = the_indices.GetCount();
    indices_type = the_indices.GetElementType();
    /* TODO
        if can do opengl 4.5
            GL::VertexArrayElementBuffer - https://docs.gl/gl4/glVertexArrayElementBuffer
        else
            Use (bind) this Vertex Array
            Use (bind) the index buffer
            Do not Use (unbind) this vertex array
            Do not Use (unbind) the index buffer
    */
    index_buffer = std::move(the_indices);
}

void GLDrawIndexed(const GLVertexArray& vertex_array) noexcept
{
    // TODO GL::DrawElements - https://docs.gl/es3/glDrawElements
}

void GLDrawVertices(const GLVertexArray& vertex_array) noexcept
{
    // TODO GL::DrawArrays - https://docs.gl/es3/glDrawArrays
}
