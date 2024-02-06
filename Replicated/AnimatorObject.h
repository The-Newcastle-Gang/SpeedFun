#include "MeshAnimation.h"

namespace NCL::CSC8503 {
    class AnimatorObject {
    public:
        AnimatorObject() {};
        ~AnimatorObject() {};
        void SetAnimation(MeshAnimation* newAnimation);
        void UpdateAnimation(float dt);

        MeshAnimation* GetAnimation() {
            return currentAnimation;
        }
    private:
        MeshAnimationInfo animationInfo;
        MeshAnimation* currentAnimation;
    };
}