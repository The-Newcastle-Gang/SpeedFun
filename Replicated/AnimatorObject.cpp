#include "AnimatorObject.h"
#include <iostream>

void AnimatorObject::SetAnimation(MeshAnimation* newAnimation) {
    
    queuedAnimation = nullptr;
    state = NONE;
    if (currentAnimation != newAnimation) { 
        ResetAnimValues(); 
    }
    currentAnimation = newAnimation;
}

void AnimatorObject::FinishTransition() {

    state = NONE;
    animationInfo = queuedAnimationInfo;
    queuedAnimationInfo.Reset();

    currentAnimation = queuedAnimation;
    queuedAnimation = nullptr;

}

void AnimatorObject::SetAnimation(std::string animationName) {
    if (animations->find(animationName) == animations->end())return;
    MeshAnimation* newAnim = (*animations)[animationName];
    state = NONE;
    if (currentAnimation != newAnim) {
        ResetAnimValues();
    }
    currentAnimation = newAnim;
}

void AnimatorObject::SetMidPose(std::string animationName) {
    if (animations->find(animationName) == animations->end())return;
    MeshAnimation* newAnim = (*animations)[animationName];
    midPoseAnimation = newAnim;
}

void AnimatorObject::UpdateAnimation(float dt) {
    animationInfo.frameTimer += dt * animationInfo.animationSpeed;
    if (animationInfo.frameTimer > currentAnimation->GetFrameTimeDelta()) {
        animationInfo.frameTimer -= currentAnimation->GetFrameTimeDelta();
        animationInfo.currentFrame = (animationInfo.currentFrame + 1) % currentAnimation->GetFrameCount();
    }
    animationInfo.framePercent = std::clamp(animationInfo.frameTimer / currentAnimation->GetFrameTimeDelta(),0.0f,1.0f);

    if (state == TRANSITIONING || state == POST_MID_TRANSITION) {
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

    else if (state == PRE_MID_TRANSITION) {
        transitionTimer += dt;
        if (transitionTimer >= transitionTime * 0.5f)state = POST_MID_TRANSITION;
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
    state = TRANSITIONING;
    animationInfo.framePercent = 0.0f;
    queuedAnimation = transitionAnim;
}

void AnimatorObject::TransitionAnimationWithMidPose(std::string animationName, float time) {
    if (currentAnimation == midPoseAnimation) { //we dont need to use the mid pose if we are already idle!
        TransitionAnimation(animationName, time);
        return;
    }

    if (animations->find(animationName) == animations->end())return;
    MeshAnimation* transitionAnim = (*animations)[animationName];
    transitionTime = time;
    transitionTimer = 0.0f;
    state = PRE_MID_TRANSITION;
    animationInfo.framePercent = 0.0f;
    queuedAnimation = transitionAnim;
}

const Matrix4* AnimatorObject::GetCurrentFrameData() {
    switch (state)
    {
        case NONE: {
            return currentAnimation->GetJointData(GetCurrentFrame());
            break;
        }
        case TRANSITIONING:{
            return currentAnimation->GetJointData(GetCurrentFrame());
            break;
        }
        case POST_MID_TRANSITION: {
            return midPoseAnimation->GetJointData(0);
            break;
        }
        case PRE_MID_TRANSITION: {
            return currentAnimation->GetJointData(GetCurrentFrame());
            break;
        }
    }
}

const Matrix4* AnimatorObject::GetNextFrameData() {
    switch (state)
    {
        case NONE: {
            return currentAnimation->GetJointData(GetNextFrame());
            break;
        }
        case TRANSITIONING: {
            return queuedAnimation->GetJointData(GetQueuedNextFrame());
            break;
        }
        case POST_MID_TRANSITION: {
            return queuedAnimation->GetJointData(GetQueuedNextFrame());
            break;
        }
        case PRE_MID_TRANSITION: {
            return midPoseAnimation->GetJointData(0);
            break;
        }
    }
}
