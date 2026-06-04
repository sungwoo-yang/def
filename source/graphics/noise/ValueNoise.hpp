/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Fade.hpp"
#include "Interpolation.hpp"
#include "NoiseCoordinate.hpp"
#include "PeriodDimension.hpp"
#include "PermutationHash.hpp"
#include "util/Random.hpp"

#include <span>
#include <vector>

namespace graphics::noise
{
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

        [[nodiscard]] std::span<T> GetValues() noexcept;

    private:
        void reset_values(size_t old_size = 0);

    private:
        PeriodDimension periodDimension = PeriodDimension::_256;
        SmoothMethod    smoothMethod    = SmoothMethod::Quintic;
        PermutationHash permutationHash{ PeriodDimension::_256 };
        std::vector<T>  values;
    };

    template <typename T>
    ValueNoise<T>::ValueNoise(PeriodDimension period, SmoothMethod smooth_method) : periodDimension(period), smoothMethod(smooth_method), permutationHash(period)
    {
        reset_values();
    }

    template <typename T>
    void ValueNoise<T>::reset_values(size_t old_size)
    {
        const auto new_size = static_cast<size_t>(periodDimension);
        values.resize(new_size);

        for (size_t i = old_size; i < values.size(); ++i)
        {
            const float random_value = util::random();
            values[i]                = T{ random_value, random_value, random_value, 1.0f };
        }
    }

    template <typename T>
    T ValueNoise<T>::Evaluate(float x) const noexcept
    {
        const NoiseCoordinate x_coord = make_noise_coord(x);
        const float           s       = fade(x_coord.interpolant, smoothMethod);

        const LinearValues<T> linear_values{ values[static_cast<size_t>(permutationHash(x_coord.base))], values[static_cast<size_t>(permutationHash(x_coord.next))] };

        return linear_mix(linear_values, s);
    }

    template <typename T>
    T ValueNoise<T>::Evaluate(float x, float y) const noexcept
    {
        const NoiseCoordinate x_coord = make_noise_coord(x);
        const NoiseCoordinate y_coord = make_noise_coord(y);

        const auto [s, t] = fade(x_coord.interpolant, y_coord.interpolant, smoothMethod);

        const BiLinearValues<T> bilinear_values{
            LinearValues<T>{ values[static_cast<size_t>(permutationHash(x_coord.base, y_coord.base))], values[static_cast<size_t>(permutationHash(x_coord.next, y_coord.base))] },
            LinearValues<T>{ values[static_cast<size_t>(permutationHash(x_coord.base, y_coord.next))], values[static_cast<size_t>(permutationHash(x_coord.next, y_coord.next))] }
        };

        return bilinear_mix(bilinear_values, s, t);
    }

    template <typename T>
    T ValueNoise<T>::Evaluate(float x, float y, float z) const noexcept
    {
        const NoiseCoordinate x_coord = make_noise_coord(x);
        const NoiseCoordinate y_coord = make_noise_coord(y);
        const NoiseCoordinate z_coord = make_noise_coord(z);

        const auto [s, t, p] = fade(x_coord.interpolant, y_coord.interpolant, z_coord.interpolant, smoothMethod);

        const TriLinearValues<T> trilinear_values{
            BiLinearValues<T>{ LinearValues<T>{ values[static_cast<size_t>(permutationHash(x_coord.base, y_coord.base, z_coord.base))],
 values[static_cast<size_t>(permutationHash(x_coord.next, y_coord.base, z_coord.base))] },
                              LinearValues<T>{ values[static_cast<size_t>(permutationHash(x_coord.base, y_coord.next, z_coord.base))],
                              values[static_cast<size_t>(permutationHash(x_coord.next, y_coord.next, z_coord.base))] } },
            BiLinearValues<T>{ LinearValues<T>{ values[static_cast<size_t>(permutationHash(x_coord.base, y_coord.base, z_coord.next))],
 values[static_cast<size_t>(permutationHash(x_coord.next, y_coord.base, z_coord.next))] },
                              LinearValues<T>{ values[static_cast<size_t>(permutationHash(x_coord.base, y_coord.next, z_coord.next))],
                              values[static_cast<size_t>(permutationHash(x_coord.next, y_coord.next, z_coord.next))] } }
        };

        return trilinear_mix(trilinear_values, s, t, p);
    }

    template <typename T>
    constexpr PeriodDimension ValueNoise<T>::GetPeriodDimension() const noexcept
    {
        return periodDimension;
    }

    template <typename T>
    void ValueNoise<T>::SetPeriod(PeriodDimension period)
    {
        if (periodDimension == period)
        {
            return;
        }

        const size_t old_size = values.size();

        periodDimension = period;
        permutationHash = PermutationHash{ periodDimension };

        reset_values(old_size);
    }

    template <typename T>
    constexpr SmoothMethod ValueNoise<T>::GetSmoothing() const noexcept
    {
        return smoothMethod;
    }

    template <typename T>
    constexpr void ValueNoise<T>::SetSmoothing(SmoothMethod smooth_method)
    {
        smoothMethod = smooth_method;
    }

    template <typename T>
    std::span<T> ValueNoise<T>::GetValues() noexcept
    {
        return std::span<T>{ values };
    }
}