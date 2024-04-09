/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
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
    GL::BindFramebuffer(GL_FRAMEBUFFER, bind ? frameBufferHandle : 0);  
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

    IF_CAN_DO_OPENGL(4, 5)
    {
        GL::CreateFramebuffers(1, &frameBufferHandle);
        if (spec.DepthFormat != GLTexture::DepthComponentSize::None)
		{
			GL::NamedFramebufferTexture(frameBufferHandle, GL_DEPTH_ATTACHMENT, depthTexture.GetHandle(), 0);
		}
        if (spec.ColorFormat != ColorComponent::None)
        {
            GL::NamedFramebufferTexture(frameBufferHandle, GL_COLOR_ATTACHMENT0, colorTexture.GetHandle(), 0);
        }

        GL::NamedFramebufferDrawBuffers(frameBufferHandle, 1, draw_buffers);
        status_result = GL::CheckNamedFramebufferStatus(frameBufferHandle, GL_FRAMEBUFFER);
    }
    else
    {
        GL::GenFramebuffers(1, &frameBufferHandle);
		GL::BindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
		if (spec.DepthFormat != GLTexture::DepthComponentSize::None)
		{
			GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.GetHandle(), 0);
		}
		if (spec.ColorFormat != ColorComponent::None)
		{
			GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture.GetHandle(), 0);
		}
		GL::DrawBuffers(1, draw_buffers);
		status_result = GL::CheckFramebufferStatus(GL_FRAMEBUFFER);
		GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    }

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
    GL::DeleteFramebuffers(1, &frameBufferHandle);
    frameBufferHandle = 0;
}
