#pragma once
#include "MeshAnimation.h"

namespace NCL {
    class AnimatorObject {
    public:
        AnimatorObject() { currentAnimation = nullptr; };
        ~AnimatorObject() {};
        void SetAnimation(MeshAnimation* newAnimation);
        void UpdateAnimation(float dt);

        MeshAnimation* GetAnimation() {
            return currentAnimation;
        }

        int GetCurrentFrame() {
            return animationInfo.currentFrame;
        }

        int GetNextFrame() {
            return (animationInfo.currentFrame + 1) % currentAnimation->GetFrameCount();
        }

        float GetFramePercent() {
            return animationInfo.framePercent;
        }
    private:
        MeshAnimationInfo animationInfo;
        MeshAnimation* currentAnimation;
    };
}