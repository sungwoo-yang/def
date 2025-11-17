#include "TargetStar.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"

TargetStar::TargetStar(Math::vec2 position)
    : CS230::GameObject(position), color(CS200::WHITE) // 기본 색상: 흰색
{
}

void TargetStar::Draw([[maybe_unused]]const Math::TransformationMatrix& camera_matrix)
{
    auto& renderer = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix({ radius, radius });
    
    // 본체 그리기
    renderer.DrawCircle(transform, color);
}

void TargetStar::OnHit()
{
    // 맞으면 노란색으로 변경
    color = 0xFFFF00FF;
}