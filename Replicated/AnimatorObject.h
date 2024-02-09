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
    private:
        MeshAnimationInfo animationInfo;
        MeshAnimation* currentAnimation;
    };
}