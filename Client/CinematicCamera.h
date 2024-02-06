#pragma once
#include "Vector3.h"
#include "Camera.h"
#include <Assets.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


namespace NCL
{
	namespace CSC8503
	{
		class CinematicCamera
		{
		public:
			void ReadPositionsFromFile(std::string filename);
			void UpdateCinematicCamera(Camera* camera);
			void GetCameraInfo(Camera* camera);
		protected:
			Vector3 Lerp(Vector3& start, Vector3 end, float time);

			std::vector<Vector3> cameraPositions;
			std::vector<float> pitches;
			std::vector<float> yaws;



			int currentCamera = 0;
			int maxCameras;


			float timer;
		};
	}
}