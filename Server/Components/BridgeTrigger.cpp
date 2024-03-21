#include "BridgeTrigger.h"

using namespace NCL;
using namespace CSC8503;

BridgeTrigger::BridgeTrigger(GameObject* go) {
	gameObject = go;
}

void BridgeTrigger::OnCollisionEnter(GameObject* otherObject) {
	if (otherObject->GetTag() == Tag::PLAYER) {
		isTrigger->TriggerOn();
	}
}