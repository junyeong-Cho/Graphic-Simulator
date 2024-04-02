/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "GLFrameBuffer.hpp"

#include "GL.hpp"
#include "environment/OpenGL.hpp"
#include <iostream>

GLFrameBuffer::~GLFrameBuffer() noexcept
{
    delete_resources();
}

GLFrameBuffer::GLFrameBuffer(GLFrameBuffer&& other) noexcept : frameBufferHandle(other.frameBufferHandle), depthTexture(std::move(other.depthTexture)), colorTexture(std::move(other.colorTexture))
{
    other.frameBufferHandle = 0;
}

GLFrameBuffer& GLFrameBuffer::operator=(GLFrameBuffer&& other) noexcept
{
    std::swap(frameBufferHandle, other.frameBufferHandle);
    std::swap(depthTexture, other.depthTexture);
    std::swap(colorTexture, other.colorTexture);
    return *this;
}

void GLFrameBuffer::Use(bool bind) const
{
    // TODO GL::BindFramebuffer - https://docs.gl/es3/glBindFramebuffer
}

void GLFrameBuffer::LoadWithSpecification(Specification spec)
{
    delete_resources();

    const int width  = spec.Width;
    const int height = spec.Height;

    if (spec.DepthFormat != GLTexture::DepthComponentSize::None)
    {
        if (const auto loaded = depthTexture.LoadAsDepthTexture(width, height, spec.DepthFormat); !loaded)
        {
            std::cerr << "Failed to create depth texture \n";
            return;
        }
    }
    else
    {
        depthTexture = GLTexture();
    }

    GLenum draw_buffers[] = { GL_NONE };

    if (spec.ColorFormat != ColorComponent::None)
    {
        if (const auto loaded = colorTexture.LoadAsRGBA(width, height); !loaded)
        {
            std::cerr << "Failed to create color texture \n";
            return;
        }
        draw_buffers[0] = GL_COLOR_ATTACHMENT0;
    }
    else
    {
        colorTexture = GLTexture();
    }


    GLenum status_result;
    /* TODO
        if can do opengl 4.5
            GL::CreateFramebuffers - https://docs.gl/gl4/glCreateFramebuffers
            if depth format is not None
                GL::NamedFramebufferTexture - add depth attachment https://docs.gl/gl4/glFramebufferTexture
            if color format is not None
                GL::NamedFramebufferTexture - add color 0 attachment
            GL::NamedFramebufferDrawBuffers - https://docs.gl/gl4/glDrawBuffers
            GL::CheckNamedFramebufferStatus - https://docs.gl/gl4/glCheckFramebufferStatus
        else
            GL::GenFramebuffers - https://docs.gl/es3/glGenFramebuffers
            GL::BindFramebuffer - https://docs.gl/es3/glBindFramebuffer
            if depth format is not None
                GL::FramebufferTexture2D - add depth attachment https://docs.gl/es3/glFramebufferTexture2D
            if color format is not None
                GL::FramebufferTexture2D - add color 0 attachment
            GL::DrawBuffers - https://docs.gl/es3/glDrawBuffers
            GL::CheckFramebufferStatus - https://docs.gl/es3/glCheckFramebufferStatus
            GL::BindFramebuffer - unbind framebuffer
    */

    if (status_result != GL_FRAMEBUFFER_COMPLETE)
    {
        std::string message = "Framebuffer is not complete.\n";
        switch (status_result)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: message += "\tERROR: Framebuffer incomplete: Attachment is NOT complete.\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: message += "\tERROR: Framebuffer incomplete: No image is attached to FBO.\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: message += "\tERROR: Framebuffer incomplete: Draw buffer.\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: message += "\tERROR: Framebuffer incomplete: Read buffer.\n"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: message += "\tERROR: Framebuffer incomplete: Unsupported by FBO implementation.\n"; break;
        }
        std::cerr << message;
        throw std::runtime_error{ message };
    }
}

void GLFrameBuffer::delete_resources() noexcept
{
    // TODO GL::DeleteFramebuffers - https://docs.gl/es3/glDeleteFramebuffers
    frameBufferHandle = 0;
}
