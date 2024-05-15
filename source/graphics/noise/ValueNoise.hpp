/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>

namespace graphics::noise
{

    enum class PeriodDimension : int
    {
        _2     = 1 << 1,  // 2^1 = 2
        _4     = 1 << 2,  // 2^2 = 4
        _8     = 1 << 3,  // 2^3 = 8
        _16    = 1 << 4,  // 2^4 = 16
        _32    = 1 << 5,  // 2^5 = 32
        _64    = 1 << 6,  // 2^6 = 64
        _128   = 1 << 7,  // 2^7 = 128
        _256   = 1 << 8,  // 2^8 = 256
        _512   = 1 << 9,  // 2^9 = 512
        _1024  = 1 << 10, // 2^10 = 1024
        _2048  = 1 << 11, // 2^11 = 2048
        _4096  = 1 << 12, // 2^12 = 4096
        _8192  = 1 << 13, // 2^13 = 8192
        _16384 = 1 << 14, // 2^14 = 16384

    };

    enum class SmoothMethod
    {
        Linear,
        Cosine,
        Smoothstep,
        Quintic
    };

    /* TODO - Implement ValueNoise Class*/
    template <typename T>
    class [[nodiscard]] ValueNoise
    {
    public:
        explicit ValueNoise(PeriodDimension period = PeriodDimension::_256, SmoothMethod smooth_method = SmoothMethod::Quintic) : period_(period), smooth_method_(smooth_method)
        {
            initialize();
        }

        [[nodiscard]] T Evaluate(float x) const noexcept
        {
            auto coord = make_noise_coord(x);
            return interpolate({ values_[coord.base % values_.size()], values_[coord.next % values_.size()] }, fade(coord.interpolant, smooth_method_));
        }

        [[nodiscard]] T Evaluate(float x, float y) const noexcept
        {
            // Assuming T is a type that supports the operations below (like glm::vec4)
            auto [s, t] = fade(x, y, smooth_method_);
            auto values = bilinear_values(x, y);
            return bilinear_mix(values, s, t);
        }

        [[nodiscard]] T Evaluate(float x, float y, float z) const noexcept
        {
            auto [s, t, p] = fade(x, y, z, smooth_method_);
            auto values    = trilinear_values(x, y, z);
            return trilinear_mix(values, s, t, p);
        }

        [[nodiscard]] constexpr PeriodDimension GetPeriodDimension() const noexcept
        {
            return period_;
        }

        void SetPeriod(PeriodDimension period)
        {
            period_ = period;
            initialize(); // Reinitialize to accommodate new period size
        }

        [[nodiscard]] constexpr SmoothMethod GetSmoothing() const noexcept
        {
            return smooth_method_;
        }

        void SetSmoothing(SmoothMethod smooth_method)
        {
            smooth_method_ = smooth_method;
        }

        [[nodiscard]] constexpr const std::vector<T>& GetValues() const noexcept
        {
            return values_;
        }

        void SetValues(std::vector<T>&& new_values)
        {
            values_ = std::move(new_values);
        }

    private:
        PeriodDimension  period_;
        SmoothMethod     smooth_method_;
        std::vector<T>   values_;
        std::vector<int> permutation_;

        void initialize()
        {
            int size = static_cast<int>(period_);
            values_.resize(size);
            permutation_.resize(size);

            std::iota(permutation_.begin(), permutation_.end(), 0);
            std::shuffle(permutation_.begin(), permutation_.end(), std::mt19937(std::random_device()()));

            std::generate(values_.begin(), values_.end(), []() { return T(util::random()); });
        }

        constexpr NoiseCoordinate make_noise_coord(float input) const noexcept
        {
            int   base        = static_cast<int>(std::floor(input));
            float interpolant = input - static_cast<float>(base);
            if (input < 0.0f && interpolant != 0.0f)
            {
                --base;
                interpolant = input - static_cast<float>(base);
            }
            int next = base + 1;
            return { base, next, interpolant };
        }

        constexpr float fade(float interpolant, SmoothMethod method) const noexcept
        {
            switch (method)
            {
                case SmoothMethod::Linear: return interpolant;
                case SmoothMethod::Cosine: return 0.5f * (1.0f - cosf(interpolant * M_PI));
                case SmoothMethod::Smoothstep: return interpolant * interpolant * (3.0f - 2.0f * interpolant);
                case SmoothMethod::Quintic: return interpolant * interpolant * interpolant * (interpolant * (interpolant * 6 - 15) + 10);
                default: return interpolant;
            }
        }
    };


}
