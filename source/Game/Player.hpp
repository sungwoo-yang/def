#pragma once

#include "Engine/GameObject.hpp"
#include "Engine/Dash.hpp"
#include "Game/Platform.hpp"
#include <vector>
#include <string>
#include <optional>
#include <memory>

// 전방 선언
class Shield; // 쉴드 컴포넌트
namespace CS200 { class IRenderer2D; }
namespace CS230 { class Sprite; }

/**
 * \brief DemoAstar의 플랫폼 이동 기능과 Dash/Sprint를 담당하는 플레이어 클래스입니다.
 *
 * 쉴드 기능은 'Shield' 컴포넌트로 분리되었습니다.
 */
class Player : public CS230::GameObject
{
public:
    Player(Math::vec2 start_pos);
    ~Player() override = default;

    // GameObject 가상 함수 오버라이드
    void Update(double dt) override;
    void Draw(const Math::TransformationMatrix& camera_matrix) override;
    GameObjectTypes Type() override { return GameObjectTypes::Ship; } // 임시
    std::string     TypeName() override { return "Player"; }

    // 물리 및 상태 업데이트 (GameState에서 호출)
    void UpdatePhysics(double dt, const std::vector<Platform>& platforms);
    void UpdateAnimation(double dt);

    // 플레이어 상태 리셋
    void ResetState();

    // 쉴드 컴포넌트 접근자
    Shield* GetShield() const { return shieldComponent; }

    // ImGui 디버깅을 위한 public 멤버
    CS230::DashComponent  dashComponent;
    bool                  isSprinting = false;
    double                shiftHoldTimer = 0.0;
    bool                  isJumping = false;
    std::optional<size_t> currentPlatformIndex = std::nullopt;
    double                velocityY = 0.0;
    bool                  faceRight = true;

private:
    // 플레이어 입력 처리 (Update 내부에서 호출)
    void HandleInput(double dt);

    // 애니메이션 관련
    enum class PlayerAnimation
    {
        None,
        Walking,
        Jump
    };

    struct AnimFrame
    {
        int    frameIndex;
        double duration;
    };

    struct AnimData
    {
        std::string            name;
        std::vector<AnimFrame> frames;
        int                    loopFrame;
    };

    void initializeAnimations();
    int  getCurrentAnimationIndex() const;

    // 컴포넌트
    Shield* shieldComponent; // 쉴드 컴포넌트 포인터 (소유권은 GameObject)

    // 물리 상태
    double                gravity              = 1500.0;
    double                jumpStrength         = 700.0;
    const double          baseSpeed            = 300.0;
    const double          sprintSpeedMultiplier = 1.8;
    const double          sprintActivationTime = 1.0;

    // 애니메이션 상태
    std::vector<AnimData> animations;
    PlayerAnimation       currentAnimation = PlayerAnimation::None;
    int                   currentFrameIndex = 0;
    double                animationTimer = 0.0;
};