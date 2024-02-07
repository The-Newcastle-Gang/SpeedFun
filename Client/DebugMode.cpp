#include "DebugMode.h"

void NCL::CSC8503::DebugMode::DisplayDebug(float dt)
{
	DisplayFPSCount(dt);
    DisplayMemoryUsage();
}

void NCL::CSC8503::DebugMode::DisplayFPSCount(float dt)
{
	Debug::Print("FPS: " + std::to_string(1/dt), Vector2(3, 50), Debug::MAGENTA);
}

void NCL::CSC8503::DebugMode::DisplayMemoryUsage()
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
	Debug::Print("Physical memory free: " + std::to_string(memoryStatus.ullAvailPhys/1024) + " KB", Vector2(3, 25), Debug::GREEN);

}
