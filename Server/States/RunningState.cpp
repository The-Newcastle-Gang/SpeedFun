#include "RunningState.h"
using namespace NCL;
using namespace CSC8503;

Running::Running(GameServer* pBaseServer) : State()
{
    baseServer = pBaseServer;
}

Running::~Running()
{

}

void Running::Update(float dt)
{

}