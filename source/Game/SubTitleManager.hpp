#pragma once

#include "Engine/Component.hpp"
#include <string>

namespace CS230
{
    class SubtitleManager : public Component
    {
    public:
        SubtitleManager();

        // 매 프레임 타이머를 감소시킵니다.
        void Update(double dt) override;
        
        // ImGui를 사용해 자막 UI를 그립니다.
        void Draw(); 
        
        // 새 자막 메시지를 표시하도록 요청합니다.
        void ShowMessage(const std::string& message, double duration = 5.0);

    private:
        std::string currentMessage;
        double      displayTimer;
    };
}