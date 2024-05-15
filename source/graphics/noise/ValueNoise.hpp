/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <vector>
#include <util/Random.hpp>


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

    struct NoiseCoordinate
    {
        int   base        = 0;
        int   next        = 0;
        float interpolant = 0.0f;
    };

    constexpr NoiseCoordinate make_noise_coord(float input) noexcept
    {
        int   base        = static_cast<int>(input);
        float interpolant = input - static_cast<float>(base);

        if (input < 0.0f && interpolant != 0.0f)
        {
            --base;
            interpolant = input - static_cast<float>(base);
        }

        int next = base + 1;

        return NoiseCoordinate{ base, next, interpolant };
    }

    constexpr float apply_smoothing(float x, SmoothMethod smoothing) noexcept
    {
        constexpr float pi = 3.14159265358979323846f;
        switch (smoothing)
        {
            case SmoothMethod::Linear: return x;
            case SmoothMethod::Cosine: return 0.5f * (1.0f - cosf(x * pi));
            case SmoothMethod::Smoothstep: return x * x * (3.0f - 2.0f * x);
            case SmoothMethod::Quintic: return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
            default: return x;
        }
    }

    constexpr float fade(float x, SmoothMethod smoothing) noexcept
    {
        return apply_smoothing(x, smoothing);
    }

    template <typename T>
    struct LinearValues
    {
        T Left{};
        T Right{};
    };

    template <typename T, typename U>
    constexpr auto linear_mix(const LinearValues<T>& values, U s) noexcept
    {
        return values.Left + s * (values.Right - values.Left);
    }

    template <typename T>
    struct BiLinearValues
    {
        LinearValues<T> Bottom{};
        LinearValues<T> Top{};
    };

    template <typename T, typename U>
    constexpr auto bilinear_mix(const BiLinearValues<T>& values, U s, U t) noexcept
    {
        auto bottom = linear_mix(values.Bottom, s);
        auto top    = linear_mix(values.Top, s);
        return bottom + t * (top - bottom);
    }

    template <typename T>
    struct TriLinearValues
    {
        BiLinearValues<T> Near{};
        BiLinearValues<T> Far{};
    };

    template <typename T, typename U>
    constexpr auto trilinear_mix(const TriLinearValues<T>& values, U s, U t, U p) noexcept
    {
        auto near = bilinear_mix(values.Near, s, t);
        auto far  = bilinear_mix(values.Far, s, t);
        return near + p * (far - near);
    }

    constexpr int period_dimension_mask(PeriodDimension period_dimension) noexcept
    {
        return static_cast<int>(period_dimension) - 1;
    }

    template <class RandomAccessIter>
    void my_random_shuffle(RandomAccessIter first, RandomAccessIter last)
    {
        std::random_device rd;
        std::mt19937       g(rd());
        std::shuffle(first, last, g);
    }

    class PermutationHash
    {
    public:
        PermutationHash() = default;

        explicit PermutationHash(PeriodDimension table_size) : period_dimension_(table_size)
        {
            int size = static_cast<int>(table_size);
            permutation_.resize(static_cast<std::vector<int>::size_type>(size * 2));

            // Initialize the first half of the permutation vector with incrementing values
            std::iota(permutation_.begin(), permutation_.begin() + size, 0);

            // Shuffle the first half
            my_random_shuffle(permutation_.begin(), permutation_.begin() + size);

            // Copy the first half to the second half
            std::copy(permutation_.begin(), permutation_.begin() + size, permutation_.begin() + size);
        }

        [[nodiscard]] int operator()(int x) const noexcept
        {
            int mask = period_dimension_mask(period_dimension_);
            return permutation_[static_cast<std::vector<int>::size_type>(x & mask)];
        }

        [[nodiscard]] int operator()(int x, int y) const noexcept
        {
            int mask = period_dimension_mask(period_dimension_);
            return permutation_[static_cast<std::vector<int>::size_type>((permutation_[static_cast<std::vector<int>::size_type>(x & mask)] + y) & mask)];
        }

        [[nodiscard]] int operator()(int x, int y, int z) const noexcept
        {
            int mask = period_dimension_mask(period_dimension_);
            return permutation_[static_cast<std::vector<int>::size_type>(
                (permutation_[static_cast<std::vector<int>::size_type>((permutation_[static_cast<std::vector<int>::size_type>(x & mask)] + y) & mask)] + z) & mask)];
        }

    private:
        std::vector<int> permutation_;
        PeriodDimension  period_dimension_;
    };

    template <typename T>
    class [[nodiscard]] ValueNoise
    {
    public:
        explicit ValueNoise(PeriodDimension period = PeriodDimension::_256, SmoothMethod smooth_method = SmoothMethod::Quintic);

        [[nodiscard]] T Evaluate(float x) const noexcept;
        [[nodiscard]] T Evaluate(float x, float y) const noexcept;
        [[nodiscard]] T Evaluate(float x, float y, float z) const noexcept;

        [[nodiscard]] constexpr PeriodDimension GetPeriodDimension() const noexcept;
        void                                    SetPeriod(PeriodDimension period);

        [[nodiscard]] constexpr SmoothMethod GetSmoothing() const noexcept;
        constexpr void                       SetSmoothing(SmoothMethod smooth_method);

        [[nodiscard]] constexpr const std::vector<T>& GetValues() const noexcept;
        constexpr void                                SetValues(std::vector<T>&& new_values);

    private:
        PeriodDimension period_;
        SmoothMethod    smooth_method_;
        std::vector<T>  values_;
        PermutationHash permutation_;

        void GenerateValues();
    };

    template <typename T>
    inline ValueNoise<T>::ValueNoise(PeriodDimension period, SmoothMethod smooth_method) : period_(period), smooth_method_(smooth_method), permutation_(period)
    {
        GenerateValues();
    }

    template <typename T>
    inline T ValueNoise<T>::Evaluate(float x) const noexcept
    {
        auto  coord = make_noise_coord(x);
        float s     = fade(coord.interpolant, smooth_method_);

        LinearValues<T> lv{ values_[static_cast<std::vector<T>::size_type>(permutation_(coord.base))], values_[static_cast<std::vector<T>::size_type>(permutation_(coord.next))] };
        return linear_mix(lv, s);
    }

    template <typename T>
    inline T ValueNoise<T>::Evaluate(float x, float y) const noexcept
    {
        auto coord_x  = make_noise_coord(x);
        auto coord_y  = make_noise_coord(y);
        auto [sx, sy] = std::make_tuple(fade(coord_x.interpolant, smooth_method_), fade(coord_y.interpolant, smooth_method_));

        BiLinearValues<T> blv{
            {values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.base, coord_y.base))], values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.next, coord_y.base))]},
            {values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.base, coord_y.next))], values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.next, coord_y.next))]}
        };

        return bilinear_mix(blv, sx, sy);
    }

    template <typename T>
    inline T ValueNoise<T>::Evaluate(float x, float y, float z) const noexcept
    {
        auto coord_x      = make_noise_coord(x);
        auto coord_y      = make_noise_coord(y);
        auto coord_z      = make_noise_coord(z);
        auto [sx, sy, sz] = std::make_tuple(fade(coord_x.interpolant, smooth_method_), fade(coord_y.interpolant, smooth_method_), fade(coord_z.interpolant, smooth_method_));

        TriLinearValues<T> tlv{
            {{ values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.base, coord_y.base, coord_z.base))],
values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.next, coord_y.base, coord_z.base))] },
             { values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.base, coord_y.next, coord_z.base))],
             values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.next, coord_y.next, coord_z.base))] }},
            {{ values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.base, coord_y.base, coord_z.next))],
values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.next, coord_y.base, coord_z.next))] },
             { values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.base, coord_y.next, coord_z.next))],
             values_[static_cast<std::vector<T>::size_type>(permutation_(coord_x.next, coord_y.next, coord_z.next))] }}
        };

        return trilinear_mix(tlv, sx, sy, sz);
    }

    template <typename T>
    inline constexpr PeriodDimension ValueNoise<T>::GetPeriodDimension() const noexcept
    {
        return period_;
    }

    template <typename T>
    inline void ValueNoise<T>::SetPeriod(PeriodDimension period)
    {
        period_      = period;
        permutation_ = PermutationHash(period);
        GenerateValues();
    }

    template <typename T>
    inline constexpr SmoothMethod ValueNoise<T>::GetSmoothing() const noexcept
    {
        return smooth_method_;
    }

    template <typename T>
    inline constexpr void ValueNoise<T>::SetSmoothing(SmoothMethod smooth_method)
    {
        smooth_method_ = smooth_method;
    }

    template <typename T>
    inline constexpr const std::vector<T>& ValueNoise<T>::GetValues() const noexcept
    {
        return values_;
    }

    template <typename T>
    inline constexpr void ValueNoise<T>::SetValues(std::vector<T>&& new_values)
    {
        values_ = std::move(new_values);
    }

    template <typename T>
    inline void ValueNoise<T>::GenerateValues()
    {
        int size = static_cast<int>(period_);
        values_.resize(static_cast<typename std::vector<T>::size_type>(size));
        for (int i = 0; i < size; ++i)
        {
            values_[static_cast<typename std::vector<T>::size_type>(i)] = static_cast<T>(util::random(0.0f, 1.0f)); // assuming T can be created from a float
        }
    }

}
