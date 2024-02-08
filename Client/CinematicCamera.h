#pragma once
#include "Vector3.h"
#include "Camera.h"
#include <Assets.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


const float MAX_TIMER = 5.0f;

namespace NCL
{
	namespace CSC8503
	{
		class CinematicCamera
		{
		public:
			void ReadPositionsFromFile(std::string filename);
			void UpdateCinematicCamera(Camera* camera, float dt);
			void WriteCameraInfo(Camera* camera, std::string filename);
		protected:
			Vector3 LerpVector3(Vector3& start, Vector3 end, float p);
			float CustomLerp(float start, float end, float p);

			std::vector<Vector3> cameraPositions;
			std::vector<float> pitches;
			std::vector<float> yaws;


			int currentCamera = 0;
			int maxCameras;
			bool isContinuous = true;

			float timer = 0;
		};
	}
}