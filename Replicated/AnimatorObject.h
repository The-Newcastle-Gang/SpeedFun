#pragma once
#include "MeshAnimation.h"
#include <map>

using namespace NCL;

class AnimatorObject {
public:
    AnimatorObject(std::map<std::string, MeshAnimation*>* a) { animations = a;  currentAnimation = nullptr; };
    ~AnimatorObject() {};
    void SetAnimation(MeshAnimation* newAnimation);
    void SetAnimation(std::string animationName);
    void UpdateAnimation(float dt);

    void TransitionAnimation(std::string animationName, float time);

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

    void GetCurrentFrameData();

    void GetNextFrameData();

    float GetFramePercent() {
        return animationInfo.framePercent;
    }
private:
    MeshAnimationInfo animationInfo;
    MeshAnimation* currentAnimation;
    MeshAnimation* queuedAnimation;
    std::map<std::string, MeshAnimation*>* animations;

    bool isTransitioning = false;
    float transitionTimer = 0.0f;
    float transitionTime = 0.0f;
};