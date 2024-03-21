#pragma once
#include "Vector3.h"
#include "Camera.h"
#include <Assets.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


const float MAX_TIMER = 1.0f;

namespace NCL
{
	namespace CSC8503
	{
		class CinematicCamera
		{
		public:
			void ReadPositionsFromFile(std::string filename);
			void UpdateCinematicCamera(Camera* camera);
			void UpdateTimer(float dt) { timer += dt; }
			void WriteCameraInfo(Camera* camera, std::string filename);

			void SetIsContinuous(bool continuous) { isContinuous = continuous; }

			void AddInitialCamera(Vector3 position);
			void ResetTimer() { timer = 0.0f; }
			void ResetCurrentCamera() { currentCamera = 0; }
			float GetTimer() { return timer; }
			void SetTimer(float t) { timer = t; }

		protected:
			Vector3 LerpVector3(Vector3& start, Vector3 end, float p);
			float LerpYaw(float start, float end, float p);

			std::vector<Vector3> cameraPositions;
			std::vector<float> pitches;
			std::vector<float> yaws;


			int currentCamera = 0;
			int maxCameras;
			bool isContinuous = false;

			float timer = 0.0f;
		};
	}
}