#include "GameStateBusy.h"
using namespace NCL;
using namespace CSC8503;

Busy::Busy() : State()
{

}

Busy::~Busy()
{

}

void Busy::Update(float dt)
{
	std::cout << "SERVER BUSY\n";
}