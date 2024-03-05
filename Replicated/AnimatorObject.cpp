#include "AnimatorObject.h"
#include <iostream>

void AnimatorObject::SetAnimation(MeshAnimation* newAnimation) {
    
    queuedAnimation = nullptr;
    isTransitioning = false;
    if (currentAnimation != newAnimation) { 
        ResetAnimValues(); 
    }
    currentAnimation = newAnimation;
}

void AnimatorObject::SetAnimation(std::string animationName) {
    if (animations->find(animationName) == animations->end())return;
    MeshAnimation* newAnim = (*animations)[animationName];
    
    if (currentAnimation != newAnim) {
        ResetAnimValues();
    }
    currentAnimation = newAnim;
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

void AnimatorObject::ResetAnimValues() {
    animationInfo.Reset();
}
