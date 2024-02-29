/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

namespace util
{
    class FPS
    {
        double timeCounter = 0;
        double numFrames   = 0;
        double fps         = 0;

    public:
        constexpr void Update(double delta_time_seconds) noexcept
        {
            ++numFrames;
            timeCounter += delta_time_seconds;
            if (timeCounter > 1.0)
            {
                fps = numFrames / timeCounter;
                timeCounter -= 1.0;
                numFrames = 0;
            }
        }

        constexpr operator long long() const noexcept
        {
            return static_cast<long long>(fps);
        }

        constexpr operator int() const noexcept
        {
            return static_cast<int>(fps);
        }
    };
}
