#include "DebugMode.h"

void DebugMode::DisplayDebug(float dt)
{
	Debug::Print("FPS: " + std::to_string(GetFPSCount(dt)), Vector2(3, 50), Debug::MAGENTA);
}

float NCL::CSC8503::DebugMode::GetFPSCount(float dt)
{
	return 1/ dt;
}
