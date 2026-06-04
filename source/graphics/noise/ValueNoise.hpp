/**
 * \file
 * \author TODO
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Fade.hpp"
#include "Interpolation.hpp"
#include "NoiseCoordinate.hpp"
#include "PeriodDimension.hpp"
#include "PermutationHash.hpp"

#include <span>

namespace graphics::noise
{
    /* TODO - Implement ValueNoise Class*/
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
    };

    // TODO remove [[maybe_unused]]
    template <typename T>
    ValueNoise<T>::ValueNoise([[maybe_unused]] PeriodDimension period, [[maybe_unused]] SmoothMethod smooth_method)
    {
        // TODO
    }

    // TODO remove [[maybe_unused]]
    template <typename T>
    T ValueNoise<T>::Evaluate([[maybe_unused]] float x) const noexcept
    {
        // TODO
        return T{};
    }

    // TODO remove [[maybe_unused]]
    template <typename T>
    T ValueNoise<T>::Evaluate([[maybe_unused]] float x, [[maybe_unused]] float y) const noexcept
    {
        // TODO
        return T{};
    }

    // TODO remvoe [[maybe_unused]]
    template <typename T>
    T ValueNoise<T>::Evaluate([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] float z) const noexcept
    {
        // TODO
        return T{};
    }

    template <typename T>
    constexpr PeriodDimension ValueNoise<T>::GetPeriodDimension() const noexcept
    {
        // TODO
        return PeriodDimension{};
    }

    // TODO remove [[maybe_unused]]
    template <typename T>
    void ValueNoise<T>::SetPeriod([[maybe_unused]] PeriodDimension period)
    {
        // TODO
    }

    template <typename T>
    constexpr SmoothMethod ValueNoise<T>::GetSmoothing() const noexcept
    {
        // TODO
        return SmoothMethod{};
    }

    // TODO remove [[maybe_unused]]
    template <typename T>
    constexpr void ValueNoise<T>::SetSmoothing([[maybe_unused]] SmoothMethod smooth_method)
    {
        // TODO
    }

    // TODO remove this variable and implement this proper
    template <typename T>
    static inline std::array<T, 1> delete_me;

    template <typename T>
    std::span<T> ValueNoise<T>::GetValues() noexcept
    {
        // TODO
        return std::span<T>{ delete_me<T> };
    }


}
