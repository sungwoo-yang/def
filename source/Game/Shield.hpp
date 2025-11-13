#pragma once
#include "Engine/Component.hpp"
#include "Engine/Vec2.hpp"
#include "CS200/RGBA.hpp"
#include <vector>
#include <utility>
#include <array>

// 전방 선언 (순환 참조 방지)
namespace CS230 { class GameObject; }
namespace CS200 { class IRenderer2D; }
namespace Math { class TransformationMatrix; }

// DemoReflection에서 가져온 색상 상수
constexpr CS200::RGBA COLOR_CYAN    = 0x00FFFFFF;
constexpr CS200::RGBA COLOR_RED     = 0xFF0000FF;
constexpr CS200::RGBA COLOR_WHITE   = 0xFFFFFFFF;

/**
 * \brief DemoReflection의 쉴드 로직을 캡슐화한 컴포넌트입니다.
 *
 * 쉴드의 회전, 패리, 피격, 상태(색상, 냉각)를 관리합니다.
 * Player(GameObject)에 부착됩니다.
 */
class Shield : public CS230::Component
{
public:
    /**
     * \brief 쉴드 컴포넌트 생성자
     * \param owner 쉴드의 소유자(Player). 쉴드는 소유자의 위치를 따라다닙니다.
     */
    Shield(CS230::GameObject* owner);

    /**
     * \brief 매 프레임 쉴드 상태를 업데이트합니다. (CS230::Component 오버라이드)
     * * 냉각 타이머, 색상 복구 등을 처리합니다.
     * GameObject::Update()에 의해 자동으로 호출됩니다.
     */
    void Update(double dt) override;

    /**
     * \brief 쉴드 관련 입력을 처리합니다. (회전, 패리 시도)
     * Player의 HandleInput에서 호출됩니다.
     */
    void HandleInput(double dt);

    /**
     * \brief 쉴드를 그립니다.
     * Player의 Draw에서 호출됩니다.
     */
    void Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const;

    /**
     * \brief 쉴드의 위치를 소유자(Player)의 현재 위치 기준으로 갱신합니다.
     */
    void UpdatePosition();

    /**
     * \brief 레이저 피격 또는 패리 성공 시 호출됩니다.
     * \param parrySuccess 패리에 성공했는지 여부
     */
    void HandleHit(bool parrySuccess);

    /**
     * \brief 레이저 시스템으로부터 패리 가능 상태를 전달받습니다.
     */
    void SetParryWindowActive(bool active) { parryWindowActive = active; }

    /**
     * \brief 현재 패리 가능 상태인지 반환합니다.
     */
    bool IsParryWindowActive() const { return parryWindowActive; }
    
    /**
     * \brief 플레이어가 패리를 시도합니다.
     * HandleInput 내부에서 호출됩니다.
     */
    void TryParry();
    
    /**
     * \brief 레이저 시스템이 패리 성공 여부를 확인할 때 호출합니다.
     * \return 플레이어가 패리를 시도했는지 여부 (이 함수 호출 시 패리 상태는 소모됨)
     */
    bool ConsumeParryState();

    /**
     * \brief 물리 계산을 위한 쉴드의 현재 선분 정보를 반환합니다.
     */
    std::vector<std::pair<Math::vec2, Math::vec2>> GetSegments() const;

    // ImGui 디버깅을 위한 public 멤버
    bool         isShieldFrozen = false;
    double       shieldFrozenTimer = 0.0;
    const double shieldFreezeDuration = 3.0;

private:
    void UpdateShieldColor(double dt);

    CS230::GameObject* owner; // 쉴드의 소유자 (Player)

    // 쉴드 기하 정보
    double     shieldAngle  = PI / 2.0;
    double     shieldLength = 100.0;
    Math::vec2 shieldStart;
    Math::vec2 shieldEnd;

    // 쉴드 색상 및 상태
    CS200::RGBA          shieldColor = COLOR_CYAN;
    std::array<float, 4> currentShieldColor{};
    std::array<float, 4> targetShieldColor{};
    
    // 피격 및 복구 타이머
    double       shieldHitTimer = 0.0;
    const double shieldColorRecoveryTime = 1.0;

    // 패링 상태
    bool         isParrying          = false; // 'Space'를 누른 순간 true
    bool         parryWindowActive   = false; // 레이저가 알려주는 패리 가능 시간
};