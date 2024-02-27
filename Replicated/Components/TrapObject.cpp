#include "TrapObject.h"

using namespace NCL;
using namespace CSC8503;

void TrapObject::Update(float dt, GameObject* go, bool isTimeout)
{
	gameObject->GetTransform().SetPosition();
	if (/*Detected collision between trap object and player*/) {
		timer += dt;
	}
	CheckTimerState();
	Disappear(isTimeout);
}

void TrapObject::CheckTimerState()
{
	if (timer >= MAX_TIMER) { isTimeout = true; }
	if (timer < MAX_TIMER) { isTimeout = false; }
}

void Disappear(bool isTimeout){
	if (isTimeout == true) {
		//Trap block disappear
	}
}
