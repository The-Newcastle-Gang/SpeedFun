#include "AnimatorObject.h"
#include <iostream>

namespace NCL {
    void AnimatorObject::SetAnimation(MeshAnimation* newAnimation) {
        currentAnimation = newAnimation;
    }

    void AnimatorObject::UpdateAnimation(float dt) {
        animationInfo.frameTimer += dt;
        if (animationInfo.frameTimer > 1.0f / currentAnimation->GetFrameRate()) {
            animationInfo.frameTimer -= 1.0f / currentAnimation->GetFrameRate();
            animationInfo.currentFrame = (animationInfo.currentFrame + 1) % currentAnimation->GetFrameCount();
        }
        std::cout << animationInfo.currentFrame << "\n";
    }
}
