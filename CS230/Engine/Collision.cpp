/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Collision.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 13, 2025
*/

#include "../Engine/Engine.h"
#include "Collision.h"
#include "GameObject.h"
#include <rlgl.h>

CS230::RectCollision::RectCollision(Math::irect boundary, GameObject* object) :
    boundary(boundary),
    object(object)
{
}

Math::rect CS230::RectCollision::WorldBoundary() {
    return {
        object->GetMatrix() * static_cast<Math::vec2>(boundary.point_1),
        object->GetMatrix() * static_cast<Math::vec2>(boundary.point_2)
    };
}

void CS230::RectCollision::Draw(Math::TransformationMatrix display_matrix) {
    const double render_height = rlGetFramebufferHeight();

    Math::rect world_boundary = WorldBoundary();

    Math::vec2 bottom_left = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Bottom() };
    Math::vec2 bottom_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Bottom() };
    Math::vec2 top_left = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Top() };
    Math::vec2 top_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Top() };

    bottom_left.y = bottom_left.y * -1 + render_height;
    bottom_right.y = bottom_right.y * -1 + render_height;
    top_left.y = top_left.y * -1 + render_height;
    top_right.y = top_right.y * -1 + render_height;

    DrawLine(int(top_left.x), int(top_left.y), int(top_right.x), int(top_right.y), WHITE);
    DrawLine(int(bottom_right.x), int(bottom_right.y), int(top_right.x), int(top_right.y), WHITE);
    DrawLine(int(bottom_right.x), int(bottom_right.y), int(bottom_left.x), int(bottom_left.y), WHITE);
    DrawLine(int(top_left.x), int(top_left.y), int(bottom_left.x), int(bottom_left.y), WHITE);
}

bool CS230::RectCollision::IsCollidingWith(GameObject* other_object) {
    Collision* other_collider = other_object->GetGOComponent<Collision>();


    if (other_collider == nullptr) {
        Engine::GetLogger().LogError("No collision component found");
        return false;
    }


    if (other_collider->Shape() != CollisionShape::Rect) {
        Engine::GetLogger().LogError("Rect vs unsupported type");
        return false;
    }

    Math::rect rectangle_1 = WorldBoundary();
    Math::rect rectangle_2 = dynamic_cast<RectCollision*>(other_collider)->WorldBoundary();


    if (rectangle_1.Right() > rectangle_2.Left() && rectangle_1.Left() < rectangle_2.Right() &&
        rectangle_1.Top() > rectangle_2.Bottom() && rectangle_1.Bottom() < rectangle_2.Top()) {
        return true;
    }
    return false;
}

bool CS230::RectCollision::IsCollidingWith(Math::vec2 point) {
    Math::rect rect = WorldBoundary();

    return (point.x >= rect.Left() && point.x < rect.Right() && point.y >= rect.Bottom() && point.y <= rect.Top());
}

CS230::CircleCollision::CircleCollision(double radius, GameObject* object) :
    radius(radius),
    object(object)
{
}

void CS230::CircleCollision::Draw(Math::TransformationMatrix display_matrix) {
    const double render_height = rlGetFramebufferHeight();
    Math::vec2 transformed_position = display_matrix * object->GetPosition();
    transformed_position.y = transformed_position.y * -1 + render_height;
    const int num_segments = 36;
    Math::vec2 previous_vertex;
    for (int i = 0; i <= num_segments + 1; i++) {
        double theta = 2.0 * PI * static_cast<double>(i) / static_cast<double>(num_segments);
        Math::vec2 vertex = {
            transformed_position.x + GetRadius() * std::cos(theta),
            transformed_position.y + GetRadius() * std::sin(theta)
        };
        if (i > 0) {
            DrawLine(int(vertex.x), int(vertex.y), int(previous_vertex.x), int(previous_vertex.y), WHITE);
        }
        previous_vertex = vertex;
    }
}

double CS230::CircleCollision::GetRadius() {
    Math::vec2 scale = object->GetScale();
    return scale.x > scale.y ? radius * scale.x : radius * scale.y;
}

bool CS230::CircleCollision::IsCollidingWith(GameObject* other_object) {
    Collision* other_collider = other_object->GetGOComponent<Collision>();


    if (other_collider == nullptr) {
        Engine::GetLogger().LogError("No collision component found");
        return false;
    }


    if (other_collider->Shape() != CollisionShape::Circle) {
        Engine::GetLogger().LogError("Rect vs unsupported type");
        return false;
    }


    CircleCollision* other = dynamic_cast<CircleCollision*>(other_collider);
    Math::vec2 A = object->GetPosition();
    Math::vec2 B = other->object->GetPosition();
    double dx = A.x - B.x;
    double dy = A.y - B.y;
    double distance = dx * dx + dy * dy;
    double sum_radius = radius + other->radius;

    return distance <= sum_radius * sum_radius;
}

bool CS230::CircleCollision::IsCollidingWith(Math::vec2 point) {
    Math::vec2 center = object->GetPosition();
    double r = GetRadius();

    Math::vec2 diff = point - center;
    double dist_squared = diff.x * diff.x + diff.y * diff.y;

    return dist_squared <= r * r;
}