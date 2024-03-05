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

void AnimatorObject::FinishTransition() {

    isTransitioning = false;
    animationInfo = queuedAnimationInfo;
    queuedAnimationInfo.Reset();

    currentAnimation = queuedAnimation;
    queuedAnimation = nullptr;

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
    if (animationInfo.frameTimer > currentAnimation->GetFrameTimeDelta()) {
        animationInfo.frameTimer -= currentAnimation->GetFrameTimeDelta();
        animationInfo.currentFrame = (animationInfo.currentFrame + 1) % currentAnimation->GetFrameCount();
    }
    animationInfo.framePercent = std::clamp(animationInfo.frameTimer / currentAnimation->GetFrameTimeDelta(),0.0f,1.0f);

    if (isTransitioning) {
        queuedAnimationInfo.frameTimer += dt * queuedAnimationInfo.animationSpeed;
        if (queuedAnimationInfo.frameTimer > queuedAnimation->GetFrameTimeDelta()) {
            queuedAnimationInfo.frameTimer -= queuedAnimation->GetFrameTimeDelta();
            queuedAnimationInfo.currentFrame = (queuedAnimationInfo.currentFrame + 1) % queuedAnimation->GetFrameCount();
        }
        queuedAnimationInfo.framePercent = std::clamp(queuedAnimationInfo.frameTimer / queuedAnimation->GetFrameTimeDelta(), 0.0f, 1.0f);

        transitionTimer += dt;
        animationInfo.framePercent = transitionTimer;
        if (transitionTimer >= transitionTime)FinishTransition();
    }

}

void AnimatorObject::ResetAnimValues() {
    animationInfo.Reset();
}

void AnimatorObject::TransitionAnimation(std::string animationName, float time) {
    if (animations->find(animationName) == animations->end())return;
    MeshAnimation* transitionAnim = (*animations)[animationName];
    transitionTime = time;
    transitionTimer = 0.0f;
    isTransitioning = true;
    animationInfo.framePercent = 0.0f;
    queuedAnimation = transitionAnim;
}

const Matrix4* AnimatorObject::GetCurrentFrameData() {
    switch (isTransitioning)
    {
        case false: {
            return currentAnimation->GetJointData(GetCurrentFrame());
            break;
        }
        case true:{
            return currentAnimation->GetJointData(GetCurrentFrame());
            break;
        }
    }
}

const Matrix4* AnimatorObject::GetNextFrameData() {
    switch (isTransitioning)
    {
        case false: {
            return currentAnimation->GetJointData(GetNextFrame());
            break;
        }
        case true: {
            return queuedAnimation->GetJointData(GetQueuedNextFrame());
            break;
        }
    }
}
