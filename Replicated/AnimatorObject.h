#pragma once
#include "MeshAnimation.h"
#include "Matrix4.h"
#include <map>
#include <algorithm>


using namespace NCL;
using namespace NCL::Maths;

class AnimatorObject {
    enum TransitionState {
        NONE,
        TRANSITIONING,
        PRE_MID_TRANSITION,
        POST_MID_TRANSITION
    };

public:
    AnimatorObject(std::map<std::string, MeshAnimation*>* a) {
        animations = a;  currentAnimation = nullptr; queuedAnimation = nullptr; midPoseAnimation = nullptr; state = NONE;
    };
    ~AnimatorObject() {};
    void SetAnimation(MeshAnimation* newAnimation);
    void SetAnimation(std::string animationName);
    void SetMidPose(std::string animationName);
    void FinishTransition();
    void UpdateAnimation(float dt);

    void TransitionAnimation(std::string animationName, float time);
    void TransitionAnimationWithMidPose(std::string animationName, float time);

    void ResetAnimValues();

    MeshAnimation* GetAnimation() {
        return currentAnimation;
    }

    int GetCurrentFrame() {
        return animationInfo.currentFrame;
    }

    int GetNextFrame() {
        return (animationInfo.currentFrame + 1) % currentAnimation->GetFrameCount();
    }

    int GetQueuedCurrentFrame() {
        return queuedAnimationInfo.currentFrame;
    }

    int GetQueuedNextFrame() {
        return (queuedAnimationInfo.currentFrame + 1) % queuedAnimation->GetFrameCount();
    }

    const Matrix4* GetCurrentFrameData();

    const Matrix4* GetNextFrameData();

    float GetFramePercent() {
        switch (state) {
            case NONE:{
                return animationInfo.framePercent;
            }
            case TRANSITIONING: {
                return std::clamp(transitionTimer / transitionTime,0.0f,1.0f);
            }
            case PRE_MID_TRANSITION: {
                return std::clamp(transitionTimer / (transitionTime*0.5f), 0.0f, 1.0f);
            }
            case POST_MID_TRANSITION: {
                return std::clamp((transitionTimer - transitionTime * 0.5f) / (transitionTime*0.5f), 0.0f, 1.0f);
            }
        }
        return 0;
    }
private:
    MeshAnimationInfo animationInfo;
    MeshAnimationInfo queuedAnimationInfo;
    MeshAnimation* currentAnimation;
    MeshAnimation* queuedAnimation;
    MeshAnimation* midPoseAnimation;
    std::map<std::string, MeshAnimation*>* animations;

    TransitionState state;

    float transitionTimer = 0.0f;
    float transitionTime = 0.0f;
};