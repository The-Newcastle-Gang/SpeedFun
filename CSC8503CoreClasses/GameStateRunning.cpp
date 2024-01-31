#include "GameStateRunning.h"
using namespace NCL;
using namespace CSC8503;

Running::Running() : State()
{

}

Running::~Running()
{

}

void Running::Update(float dt)
{
	std::cout << "SERVER RUNNING\n";
}