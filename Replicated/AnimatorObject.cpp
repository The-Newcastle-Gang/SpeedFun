#include "AnimatorObject.h"
#include <iostream>

void AnimatorObject::SetAnimation(MeshAnimation* newAnimation) {
    currentAnimation = newAnimation;
    queuedAnimation = nullptr;
    isTransitioning = false;
}

void AnimatorObject::UpdateAnimation(float dt) {
    animationInfo.frameTimer += dt * animationInfo.animationSpeed;
    if (animationInfo.frameTimer >currentAnimation->GetFrameTimeDelta()) {
        animationInfo.frameTimer -= currentAnimation->GetFrameTimeDelta();
        animationInfo.currentFrame = (animationInfo.currentFrame + 1) % currentAnimation->GetFrameCount();
    }
    //std::cout << animationInfo.currentFrame << "\n";
    animationInfo.framePercent = animationInfo.frameTimer / currentAnimation->GetFrameTimeDelta();
}
