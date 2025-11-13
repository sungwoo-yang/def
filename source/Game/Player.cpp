#include "Game/Player.hpp"
#include "Game/Shield.hpp" // 쉴드 컴포넌트 포함
#include "Engine/GameObject.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "CS200/IRenderer2D.hpp"
#include <cmath>
#include <limits>
#include <algorithm>

namespace
{
    // 플레이어 스프라이트 정보 (DemoAstar)
    constexpr Math::ivec2 PLAYER_FRAME_SIZE{ 63, 127 };
    constexpr Math::ivec2 PLAYER_HOT_SPOT{ 32, 63 };
} 

Player::Player(Math::vec2 start_pos) : CS230::GameObject(start_pos)
{
    // 스프라이트 컴포넌트 추가
    AddGOComponent(new CS230::Sprite("Assets/images/DemoAstar/Robot.png", this));
    
    // 쉴드 컴포넌트 생성 및 GameObject의 컴포넌트 매니저에 등록
    shieldComponent = new Shield(this);
    AddGOComponent(shieldComponent); 

    // 애니메이션 초기화
    initializeAnimations();
    
    // 대시 설정
    dashComponent.dashCooldown = 3.0;

    // 상태 리셋
    ResetState();
}

void Player::ResetState()
{
    // 플레이어 위치 및 물리 상태 리셋 (DemoAstar)
    SetPosition({ 400.0, 100.0 + PLAYER_HOT_SPOT.y + 100.0 });
    velocityY            = 0.0;
    isJumping            = true;
    currentPlatformIndex = std::nullopt;
    faceRight            = true;

    // 애니메이션 리셋
    currentAnimation  = PlayerAnimation::None;
    currentFrameIndex = 0;
    animationTimer    = 0.0;

    // 대시/스프린트 리셋
    dashComponent.isDashing         = false;
    dashComponent.dashTimer         = 0.0;
    dashComponent.dashCooldownTimer = 0.0;
    isSprinting                     = false;
    shiftHoldTimer                  = 0.0;

    // 쉴드 컴포넌트 리셋 (새 Shield 객체를 만들거나 Reset 함수 호출)
    // 여기서는 기존 컴포넌트를 제거하고 새로 만듭니다.
    RemoveGOComponent<Shield>();
    shieldComponent = new Shield(this);
    AddGOComponent(shieldComponent);
}

void Player::Update(double dt)
{
    // 1. 플레이어 입력 처리
    HandleInput(dt);

    // 2. 대시 타이머 업데이트
    dashComponent.UpdateTimers(dt);

    // 3. GameObject의 Update 호출
    // 이것이 ComponentManager를 통해 shieldComponent->Update(dt)를 호출합니다.
    GameObject::Update(dt); 
    
    // 4. 플레이어 애니메이션 업데이트
    UpdateAnimation(dt);
}

void Player::HandleInput(double dt)
{
    auto& input = CS230::Engine::GetInput();

    // 1. 쉴드 입력 처리 (쉴드 컴포넌트에 위임)
    // 쉴드 회전(방향키), 패리(스페이스) 입력을 처리
    if (shieldComponent)
    {
        shieldComponent->HandleInput(dt);
    }

    // 2. 대시/스프린트 입력 (LShift)
    const bool shiftDown         = input.KeyDown(CS230::Input::Keys::LShift);
    const bool shiftJustReleased = input.KeyJustReleased(CS230::Input::Keys::LShift);
    bool canStartSpecialMove = dashComponent.dashCooldownTimer <= 0.0 && !dashComponent.IsDashing() && !isSprinting;

    if (shiftDown)
    {
        if (canStartSpecialMove)
        {
            shiftHoldTimer += dt;
            if (shiftHoldTimer >= sprintActivationTime)
            {
                isSprinting = true; 
            }
        }
    }
    else if (shiftJustReleased)
    {
        // 짧게 눌렀다 뗐을 때 (대시)
        if (shiftHoldTimer > 0.0 && shiftHoldTimer < sprintActivationTime) 
        {
            if (canStartSpecialMove)
            {
                dashComponent.TryStartDash(faceRight);
            }
        }
        isSprinting    = false;
        shiftHoldTimer = 0.0;
    }
    else 
    {
        isSprinting    = false;
        shiftHoldTimer = 0.0;
    }

    // 3. 이동 및 점프 입력 (대시 중이 아닐 때)
    if (!dashComponent.IsDashing())
    {
        double currentBaseSpeed = baseSpeed;
        if (isSprinting)
        {
            currentBaseSpeed *= sprintSpeedMultiplier;
        }
        
        Math::vec2 move{ 0.0, 0.0 };
        // 이동 (A/D)
        if (input.KeyDown(CS230::Input::Keys::A))
        {
            move.x -= 1.0;
            faceRight = false;
        }
        if (input.KeyDown(CS230::Input::Keys::D))
        {
            move.x += 1.0;
            faceRight = true;
        }
        SetVelocity({ move.x * currentBaseSpeed, velocityY }); // Y 속도는 물리에서 계산된 값 유지

        // 점프 (W 또는 Up) - 패리(Space)와 키 분리
        if (!isJumping && (input.KeyJustPressed(CS230::Input::Keys::W) || input.KeyJustPressed(CS230::Input::Keys::Up)))
        {
            velocityY            = jumpStrength;
            isJumping            = true;
            currentPlatformIndex = std::nullopt;
            CS230::Engine::GetLogger().LogEvent("Event: Player Jump");
        }
    }
}

void Player::UpdatePhysics(double dt, const std::vector<Platform>& platforms)
{
    // 중력 적용
    bool applyGravity = true;
    if (dashComponent.IsDashing() && dashComponent.disableGravityOnDash)
    {
        applyGravity = false;
    }
    else if (!isJumping && currentPlatformIndex.has_value())
    {
        applyGravity = false;
        velocityY = 0.0;
    }
    
    if (applyGravity)
    {
        velocityY -= gravity * dt;
    }

    // 현재 속도 계산
    Math::vec2 currentVelocity = GetVelocity(); // 이동(A/D) 속도
    if (dashComponent.IsDashing())
    {
        currentVelocity.x = dashComponent.GetDashVelocityX(); // 대시 속도 오버라이드
    }
    currentVelocity.y = velocityY; // 중력/점프 속도 적용

    // 위치 예측
    double     previousFeetY     = GetPosition().y - PLAYER_HOT_SPOT.y;
    Math::vec2 predictedPosition = GetPosition();
    
    predictedPosition.x += currentVelocity.x * dt;
    if (!dashComponent.IsDashing() || !dashComponent.disableGravityOnDash)
    {
        predictedPosition.y += currentVelocity.y * dt;
    }

    double predictedFeetY = predictedPosition.y - PLAYER_HOT_SPOT.y;

    // 플랫폼 충돌 검사
    bool                  landedOnPlatform    = false;
    double                landingPlatformY    = -std::numeric_limits<double>::infinity();
    std::optional<size_t> landedPlatformIndex = std::nullopt;
    const double          landingEpsilon      = 1.0; // 착지 오차 범위

    // 드롭다운 검사
    bool tryingToDropDown = false;
    auto& input = CS230::Engine::GetInput();
    if (!dashComponent.IsDashing() && currentPlatformIndex.has_value() && 
        platforms[*currentPlatformIndex].canDropDown && 
        input.KeyDown(CS230::Input::Keys::S) && !isJumping)
    {
        tryingToDropDown              = true;
        isJumping            = true;
        currentPlatformIndex = std::nullopt;
        predictedPosition.y -= 50.0; // 플랫폼에서 즉시 살짝 내려옴
        CS230::Engine::GetLogger().LogEvent("Event: Player Drop Down");
    }

    // 착지 검사 (떨어지는 중이고, 드롭다운 중이 아니고, 대시 중이 아닐 때)
    if (velocityY <= 0 && !tryingToDropDown && !dashComponent.IsDashing())
    {
        for (size_t i = 0; i < platforms.size(); ++i)
        {
            const auto& platform  = platforms[i];
            double      platformY = platform.start.y; // 플랫폼의 Y 위치

            // 발 위치가 이전 프레임에는 플랫폼 위에, 이번 프레임에는 플랫폼 아래에 있는지
            if (previousFeetY >= platformY - landingEpsilon && predictedFeetY <= platformY + landingEpsilon && 
                // X축 범위 내에 있는지
                predictedPosition.x >= platform.start.x - (PLAYER_FRAME_SIZE.x / 2.0 - PLAYER_HOT_SPOT.x) &&
                predictedPosition.x <= platform.end.x + (PLAYER_FRAME_SIZE.x / 2.0 - PLAYER_HOT_SPOT.x))
            {
                // 여러 플랫폼 중 가장 높은 플랫폼에 착지
                if (platformY > landingPlatformY) 
                {
                    landingPlatformY    = platformY;
                    landedPlatformIndex = i;
                    landedOnPlatform    = true;
                }
            }
        }
    }

    // 착지 처리
    if (landedOnPlatform)
    {
        if (isJumping) // 이전에 점프 상태였다면 (즉, 방금 착지했다면)
        {
            CS230::Engine::GetLogger().LogEvent("Event: Player Landed on platform " + std::to_string(*landedPlatformIndex));
        }
        predictedPosition.y   = landingPlatformY + PLAYER_HOT_SPOT.y; // 발 위치를 플랫폼 높이로
        velocityY             = 0.0;
        isJumping             = false;
        currentPlatformIndex  = landedPlatformIndex;
    }
    // 공중에 있는 경우
    else if (!tryingToDropDown)
    {
        if (currentPlatformIndex.has_value()) // 이전에 플랫폼에 있었다면 (즉, 방금 떨어졌다면)
        {
            CS230::Engine::GetLogger().LogEvent("Event: Player Fell off platform");
        }
        currentPlatformIndex = std::nullopt; 
        isJumping = true;
    }

    // 최종 위치 및 속도 반영
    SetPosition(predictedPosition);
    SetVelocity({ currentVelocity.x, velocityY });
}


void Player::UpdateAnimation(double dt)
{
    const PlayerAnimation prevAnim = currentAnimation;
    
    // 애니메이션 상태 결정
    if (dashComponent.IsDashing()) 
    {
        currentAnimation = PlayerAnimation::Jump; 
    }
    else if (isJumping) 
    {
        currentAnimation = PlayerAnimation::Jump;
    }
    else if (std::abs(GetVelocity().x) > 0.01) // 걷는 중
    {
        currentAnimation = PlayerAnimation::Walking;
    }
    else // 정지
    {
        currentAnimation = PlayerAnimation::None;
    }

    // 애니메이션이 바뀌면 프레임 리셋
    if (currentAnimation != prevAnim)
    {
        currentFrameIndex = 0;
        animationTimer      = 0.0;
    }

    // 애니메이션 타이머 업데이트
    const size_t anim_index = getCurrentAnimationIndex();
    const auto& anim = animations[anim_index];
    if (anim.frames.empty()) return; 

    if (currentFrameIndex < 0 || static_cast<size_t>(currentFrameIndex) >= anim.frames.size())
    {
        currentFrameIndex = anim.loopFrame;
        animationTimer      = 0.0; 
    }

    animationTimer += dt;
    const auto& current_frame = anim.frames[static_cast<size_t>(currentFrameIndex)];

    // 다음 프레임으로 넘어갈 시간인지 확인
    if (animationTimer >= current_frame.duration)
    {
        animationTimer -= current_frame.duration; 
        currentFrameIndex++;
        // 애니메이션 끝에 도달하면 루프 프레임으로
        if (static_cast<size_t>(currentFrameIndex) >= anim.frames.size())
        {
            currentFrameIndex = anim.loopFrame; 
        }
    }
}


void Player::Draw(const Math::TransformationMatrix& camera_matrix)
{
    // 1. 플레이어 스프라이트 그리기 (애니메이션 적용)
    CS230::Sprite* sprite = GetGOComponent<CS230::Sprite>();
    if (sprite)
    {
        const size_t anim_index = getCurrentAnimationIndex();
        const auto& anim = animations[anim_index];
        if (anim.frames.empty()) return;

        int frameIdx = currentFrameIndex;
        if (frameIdx < 0 || static_cast<size_t>(frameIdx) >= anim.frames.size())
        {
            frameIdx = anim.loopFrame;
        }

        const int  sprite_frame = anim.frames[static_cast<size_t>(frameIdx)].frameIndex;
        const auto texel_base   = Math::ivec2{ sprite_frame * PLAYER_FRAME_SIZE.x, 0 };

        // HotSpot, 방향(Scale), 위치(Translate) 적용
        const auto to_center = Math::TranslationMatrix(Math::vec2{ static_cast<double>(-PLAYER_HOT_SPOT.x), static_cast<double>(-PLAYER_HOT_SPOT.y) });
        const auto scale     = faceRight ? Math::ScaleMatrix({ 1.0, 1.0 }) : Math::ScaleMatrix({ -1.0, 1.0 });
        const auto translate = Math::TranslationMatrix(GetPosition());
        const auto transform = translate * scale * to_center;

        // 최종 변환(카메라 * 오브젝트)으로 스프라이트 그리기
        sprite->Draw(camera_matrix * transform);
    }

    // 2. 쉴드 그리기 (쉴드 컴포넌트에 위임)
    // Shield::Draw는 컴포넌트 매니저를 통해 자동으로 호출되지 않으므로,
    // Player::Draw에서 명시적으로 호출해야 합니다.
    if (shieldComponent)
    {
        shieldComponent->Draw(CS230::Engine::GetRenderer2D(), camera_matrix);
    }

    // 3. GameObject의 기본 Draw (디버그 충돌 영역 등)
    GameObject::Draw(camera_matrix);
}

void Player::initializeAnimations()
{
    animations.clear();
    animations.resize(3); // None, Walking, Jump

    // 0: Idle
    animations[static_cast<int>(PlayerAnimation::None)] = { "Idle", { { 0, 0.5 } }, 0 };
    // 1: Walk
    animations[static_cast<int>(PlayerAnimation::Walking)] = { "Walk", { { 0, 0.2 }, { 1, 0.2 } }, 0 };
    // 2: Jump
    animations[static_cast<int>(PlayerAnimation::Jump)] = { "Jump", { { 2, 1.0 } }, 0 }; 

    currentAnimation = PlayerAnimation::None;
    currentFrameIndex = 0;
    animationTimer = 0.0;
}

int Player::getCurrentAnimationIndex() const
{
    int index = static_cast<int>(currentAnimation);
    if(index < 0 || index >= static_cast<int>(animations.size()))
    {
        return 0; // 'None' 애니메이션
    }
    return index;
}