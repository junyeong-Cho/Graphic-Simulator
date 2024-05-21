/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include <array>
#include <filesystem>

#include "GLHandle.hpp"
#include "GLTexture.hpp"

#pragma once

class [[nodiscard]] GLFrameBuffer
{
public:
    GLFrameBuffer() = default;
    ~GLFrameBuffer() noexcept;

    GLFrameBuffer(const GLFrameBuffer& other) = delete;
    GLFrameBuffer(GLFrameBuffer&& other) noexcept;
    GLFrameBuffer& operator=(const GLFrameBuffer& other) = delete;
    GLFrameBuffer& operator=(GLFrameBuffer&& other) noexcept;

    void Use(bool bind = true) const;

    [[nodiscard]] GLHandle GetHandle() const noexcept
    {
        return frameBufferHandle;
    }

    const GLTexture& DepthTexture() const
    {
        return depthTexture;
    }

    const GLTexture& ColorTexture() const
    {
        return colorTexture;
    }

    GLTexture& ColorTexture()
    {
        return colorTexture;
    }

    enum ColorComponent
    {
        None,
        RGBA8   = GLTexture::ColorFormat::RGBA8,
        RGBA32F = GLTexture::ColorFormat::RGBA32F,
        R32F    = GLTexture::ColorFormat::R32F,
    };

    struct Specification
    {
        int                           Width = 0, Height = 0;
        GLTexture::DepthComponentSize DepthFormat = GLTexture::DepthComponentSize::None;
        ColorComponent                ColorFormat = ColorComponent::None;
    };

    void LoadWithSpecification(Specification spec);

private:
    void delete_resources() noexcept;

private:
    GLHandle  frameBufferHandle = 0;
    GLTexture depthTexture;
    GLTexture colorTexture;
};
