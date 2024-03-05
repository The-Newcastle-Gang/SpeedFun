#pragma once
#include "MeshAnimation.h"
#include "Matrix4.h"
#include <map>
#include <algorithm>


using namespace NCL;
using namespace NCL::Maths;

class AnimatorObject {
public:
    AnimatorObject(std::map<std::string, MeshAnimation*>* a) { animations = a;  currentAnimation = nullptr; queuedAnimation = nullptr;};
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

    const Matrix4* GetCurrentFrameData();

    const Matrix4* GetNextFrameData();

    float GetFramePercent() {
        switch (isTransitioning) {
            case false:{
                return animationInfo.framePercent;
            }
            case true: {
                return std::clamp(transitionTime / transitionTimer,0.0f,1.0f);
            }
        }
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