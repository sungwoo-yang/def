#pragma once
#include "Engine/Rect.hpp" // <--- 님의 엔진 경로로 수정
#include "Engine/Vec2.hpp" // <--- 님의 엔진 경로로 수정
#include <string>
#include <vector>

// Math 네임스페이스 안에 vec2와 rect가 있으므로 using 사용
using Math::rect;
using Math::vec2;

struct [[nodiscard]] Polygon
{
    std::vector<vec2> vertices;
    int               vertexCount = 0; // 3 대신 0으로 초기화
    int               polycount   = 0;
    std::string       polyindex   = "";

    vec2 FindCenter()
    {
        if (vertices.empty())
        {
            return { 0, 0 };
        }

        vec2 center = { 0, 0 };
        for (vec2& vertice : vertices)
        {
            center.x += vertice.x;
            center.y += vertice.y;
        }

        vertexCount = static_cast<int>(vertices.size()); // <--- 실제 크기로 설정

        center.x /= vertexCount;
        center.y /= vertexCount;
        return center;
    }

    Math::rect FindBoundary()
    {
        if (vertices.empty())
        {
            return Math::rect{};
        }
        vec2 minPoint = vertices[0];
        vec2 maxPoint = vertices[0];
        for (const auto& vertex : vertices)
        {
            minPoint.x = std::min(minPoint.x, vertex.x);
            minPoint.y = std::min(minPoint.y, vertex.y);
            maxPoint.x = std::max(maxPoint.x, vertex.x);
            maxPoint.y = std::max(maxPoint.y, vertex.y);
        }
        return Math::rect{ minPoint, maxPoint };
    }
};