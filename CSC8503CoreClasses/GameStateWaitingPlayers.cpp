#include "GameStateWaitingPlayers.h"
using namespace NCL;
using namespace CSC8503;

WaitingPlayers::WaitingPlayers() : State()
{

}

WaitingPlayers::~WaitingPlayers()
{

}

void WaitingPlayers::Update(float dt)
{
	std::cout << "SERVER WAITING FOR PLAYERS\n";
}