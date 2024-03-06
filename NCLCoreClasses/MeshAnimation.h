/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <vector>
#include <string>

namespace NCL {
	namespace Maths {
		class Matrix4;
	}

    struct MeshAnimationInfo {
        int currentFrame;
        float frameTimer;
        float animationSpeed; //used to modify frameRate of an animation
        float framePercent;

        MeshAnimationInfo() {
            frameTimer = 0;
            currentFrame = 0;
            animationSpeed = 1.0f;
            framePercent = 0.0f;
        }

        MeshAnimationInfo(float animationSpeed) {
            frameTimer = 0;
            currentFrame = 0;
            this->animationSpeed = animationSpeed;
            framePercent = 0.0f;
        }

        void Reset() {
            frameTimer = 0;
            currentFrame = 0;
            animationSpeed = 1;
            framePercent = 0.0f;
        }
    };

	class MeshAnimation	{
	public:
		MeshAnimation();

		MeshAnimation(unsigned int jointCount, unsigned int frameCount, float frameRate, std::vector<Maths::Matrix4>& frames);

		MeshAnimation(const std::string& filename);
		virtual ~MeshAnimation();

		unsigned int GetJointCount() const {
			return jointCount;
		}

		unsigned int GetFrameCount() const {
			return frameCount;
		}

		float GetFrameRate() const {
			return frameRate;
		}


        float GetFrameTimeDelta() const {
            return frameTimeDelta;
        }

		const Maths::Matrix4* GetJointData(unsigned int frame) const;

	protected:
		unsigned int	jointCount;
		unsigned int	frameCount;
		float			frameRate;
        float frameTimeDelta;

		std::vector<Maths::Matrix4>		allJoints;
	};
}

