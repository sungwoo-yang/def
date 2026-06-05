/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2024 Spring
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
            // TODO fill in column vectors of the View Matrix (world space to camera space)
            //      You do not need any external functions except for doing a dot product
            const glm::vec4 column_1{};
            const glm::vec4 column_2{};
            const glm::vec4 column_3{};
            const glm::vec4 column_4{};
            return glm::mat4{ column_1, column_2, column_3, column_4 };
        }

        [[nodiscard]] constexpr glm::mat4 ToWorldMatrix() const noexcept
        {
            // TODO fill in column vectors of the Inverse of the View Matrix (camera space to world space)
            //      DO NOT invoke any external functions like glm::inverse. It is not needed!
            const glm::vec4 column_1{};
            const glm::vec4 column_2{};
            const glm::vec4 column_3{};
            const glm::vec4 column_4{};
            return glm::mat4{ column_1, column_2, column_3, column_4 };
        }
    };
}
