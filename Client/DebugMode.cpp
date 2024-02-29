#include "DebugMode.h"
#include <sysinfoapi.h>

using namespace NCL;
using namespace CSC8503;


void NCL::CSC8503::DebugMode::DisplayDebug(float dt)
{
	GetFPSCount(dt);
	GetMemoryUsage();
}

void DebugMode::GetFPSCount(float dt)
{
	Debug::Print("FPS: " + std::to_string(1 / dt), Vector2(3, 50), Debug::MAGENTA);
}

void DebugMode::GetMemoryUsage()
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

void DebugMode::GetCameraInfo()
{
}
