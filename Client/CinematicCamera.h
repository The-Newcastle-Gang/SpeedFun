#pragma once
#include "Vector3.h"
#include "Camera.h"
#include <Assets.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


const float LERP_MULTIPLIER = 0.15f;


namespace NCL
{
	namespace CSC8503
	{
		class CinematicCamera
		{
		public:
			void ReadPositionsFromFile(std::string filename);
			void UpdateCinematicCamera(Camera* camera);
			void WriteCameraInfo(Camera* camera, std::string filename);
		protected:
			Vector3 LerpVector3(Vector3& start, Vector3 end, float time);

			std::vector<Vector3> cameraPositions;
			std::vector<float> pitches;
			std::vector<float> yaws;



			int currentCamera = 0;
			int maxCameras;


			float timer;
		};
	}
}