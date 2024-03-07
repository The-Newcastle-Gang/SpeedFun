#include "DebugMode.h"
#include <sysinfoapi.h>

using namespace NCL;
using namespace CSC8503;


void DebugMode::UpdateDebugMode(float dt)
{
	DisplayFPSCount(dt);
	DisplayMemoryUsage();
	DisplayCameraInfo();
}

void DebugMode::DisplayFPSCount(float dt)
{
	Debug::Print("FPS: " + std::to_string(1 / dt), Vector2(3, 50), Debug::MAGENTA);
}

void DebugMode::DisplayMemoryUsage()
{
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(memoryStatus);
	GlobalMemoryStatusEx(&memoryStatus);

	Debug::Print("Percentage of memory used: " + std::to_string(memoryStatus.dwMemoryLoad) + "%", Vector2(3, 5), Debug::GREEN);

	DWORDLONG virtaulMemoryUsed = (memoryStatus.ullTotalVirtual - memoryStatus.ullAvailVirtual) / 1024;
	Debug::Print("Virtual memory used: " + std::to_string(virtaulMemoryUsed) + " KB", Vector2(3, 10), Debug::RED);
	Debug::Print("Virtual memory free: " + std::to_string(memoryStatus.ullAvailVirtual / 1024) + " KB", Vector2(3, 15), Debug::GREEN);

	DWORDLONG physMemoryUsed = (memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys) / 1024;
	Debug::Print("Physical memory used: " + std::to_string(physMemoryUsed) + " KB", Vector2(3, 20), Debug::RED);
	Debug::Print("Physical memory free: " + std::to_string(memoryStatus.ullAvailPhys / 1024) + " KB", Vector2(3, 25), Debug::GREEN);

}

void DebugMode::DisplayCameraInfo()
{
	Debug::Print("Camera info:", Vector2(75, 35), Debug::WHITE);
	Debug::Print("X: " + std::to_string(currentCam->GetPosition().x), Vector2(75, 30), Debug::WHITE);
	Debug::Print("Y: " + std::to_string(currentCam->GetPosition().y), Vector2(75, 25), Debug::WHITE);
	Debug::Print("Z: " + std::to_string(currentCam->GetPosition().z), Vector2(75, 20), Debug::WHITE);
	Debug::Print("Roll: " + std::to_string(currentCam->GetRoll()), Vector2(75, 15), Debug::WHITE);
	Debug::Print("Pitch: " + std::to_string(currentCam->GetPitch()), Vector2(75, 10), Debug::WHITE);
	Debug::Print("Yaw: " + std::to_string(currentCam->GetYaw()), Vector2(75, 5), Debug::WHITE);
}
