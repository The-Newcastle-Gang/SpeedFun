#include "GameStateInactive.h"
using namespace NCL;
using namespace CSC8503;

Inactive::Inactive() : State()
{

}

Inactive::~Inactive()
{

}

void Inactive::Update(float dt)
{
	std::cout << "INACTIVE\n";
}