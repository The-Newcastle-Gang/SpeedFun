#include "AnimatorObject.h"

namespace NCL::CSC8503 {
    void AnimatorObject::SetAnimation(MeshAnimation* newAnimation) {
        currentAnimation = newAnimation;
    }

    void AnimatorObject::UpdateAnimation(float dt) {
        animationInfo.frameTimer += dt;
        if (animationInfo.frameTimer > 1.0f / currentAnimation->GetFrameRate()) {
            animationInfo.frameTimer -= currentAnimation->GetFrameRate();
        }
    }
}
