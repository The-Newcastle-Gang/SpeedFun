#include "DebugMode.h"
#include <sysinfoapi.h>

using namespace NCL;
using namespace CSC8503;

Camera* DebugMode::currentCam;

static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int ProcessorsNum;
static HANDLE self;

double DebugMode::OurCurrentUsage = 0.0;


void DebugMode::SetDebugCam(Camera *cam) {
        currentCam = cam;
}

void DebugMode::InitDebugInfo() {
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysInfo);
    ProcessorsNum = sysInfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    self = GetCurrentProcess();
    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}

double DebugMode::GetCurrentCPUVal(){
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (sys.QuadPart - lastSysCPU.QuadPart) +
              (user.QuadPart - lastUserCPU.QuadPart);
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent /= ProcessorsNum;
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;
    return percent * 100;
}


void DebugMode::UpdateDebugMode(float dt)
{
    OurCurrentUsage = GetCurrentCPUVal(); //somehow ignore the 0s??
	DisplayFPSCount(dt);
	DisplayMemoryUsage();
	DisplayCameraInfo();
    DisplayCollisionInfo();
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
	Debug::Print("Virtual memory used: " + std::to_string(virtaulMemoryUsed) + " KB (" + std::to_string(virtaulMemoryUsed/(1024 * 1024)) + " GB)", Vector2(3, 10), Debug::RED);
	Debug::Print("Virtual memory free: " + std::to_string(memoryStatus.ullAvailVirtual / 1024) + " KB (" + std::to_string(memoryStatus.ullAvailVirtual / (1024 * 1024 * 1024)) + " GB)", Vector2(3, 15), Debug::GREEN);

	DWORDLONG physMemoryUsed = (memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys) / 1024;
	Debug::Print("Physical memory used: " + std::to_string(physMemoryUsed) + " KB (" + std::to_string(physMemoryUsed / (1024 * 1024)) + " GB)", Vector2(3, 20), Debug::RED);
	Debug::Print("Physical memory free: " + std::to_string(memoryStatus.ullAvailPhys / 1024) + " KB (" + std::to_string(memoryStatus.ullAvailPhys / (1024 * 1024 * 1024)) + " GB)", Vector2(3, 25), Debug::GREEN);

    // TODO: CPU USED BY OUR CURRENT PROCESS
    if(OurCurrentUsage > 0){
        std::cout << OurCurrentUsage << std::endl;
    }
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

//display COllision volumes
void DebugMode::DisplayCollisionInfo(){
//    Debug::Print("Collisions hehe", Vector2(50,50), Debug::GREEN);
}

