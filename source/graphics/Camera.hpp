/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include <glm/ext/matrix_transform.hpp> // translate, rotate
#include <glm/geometric.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace graphics
{
    struct Camera
    {
        using Point  = glm::vec3;
        using Vector = glm::vec3;

        static constexpr Vector WORLD_UP{ 0, 1, 0 };

        Point  Eye{ 0.0f };
        Vector Right{ 1, 0, 0 };
        Vector Up{ 0, 1, 0 };
        Vector Back{ 0, 0, 1 };

        constexpr Camera() noexcept = default;

        Camera(Point position, Vector look_dir, Vector relative_up) noexcept
        {
            Eye   = position;
            Right = glm::normalize(glm::cross(look_dir, relative_up));
            Up    = glm::normalize(glm::cross(Right, look_dir));
            Back  = glm::normalize(-look_dir);
        }

        constexpr void Rotate(float yaw, float pitch) noexcept
        {
            if (yaw != 0)
            {
                const auto rotate = glm::mat3(glm::rotate(glm::mat4(1.0f), yaw, WORLD_UP));
                Up                = rotate * Up;
                Right             = rotate * Right;
                Back              = rotate * Back;
            }

            if (pitch != 0)
            {
                const auto rotate = glm::mat3(glm::rotate(glm::mat4(1.0f), pitch, Right));
                Up                = rotate * Up;
                Back              = rotate * Back;
            }
        }

        constexpr void Move(Vector delta) noexcept
        {
            Eye += Right * delta.x;
            Eye += WORLD_UP * delta.y;
            Eye += Back * (-delta.z);
        }

        [[nodiscard]] constexpr glm::mat4 ViewMatrix() const noexcept
        {
            const glm::vec4 column_1{ Right.x, Up.x, Back.x, 0.0f };
            const glm::vec4 column_2{ Right.y, Up.y, Back.y, 0.0f };
            const glm::vec4 column_3{ Right.z, Up.z, Back.z, 0.0f };
            const glm::vec4 column_4{ -glm::dot(Right, Eye), -glm::dot(Up, Eye), -glm::dot(Back, Eye), 1.0f };

            return glm::mat4{ column_1, column_2, column_3, column_4 };
        }

        [[nodiscard]] constexpr glm::mat4 ToWorldMatrix() const noexcept
        {
            const glm::vec4 column_1{ Right, 0.0f };
            const glm::vec4 column_2{ Up, 0.0f };
            const glm::vec4 column_3{ Back, 0.0f };
            const glm::vec4 column_4{ Eye, 1.0f };

            return glm::mat4{ column_1, column_2, column_3, column_4 };
        }
    };
}
