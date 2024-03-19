#include "Component.h"
#include "GameObject.h"
#pragma once

using namespace NCL;
using namespace CSC8503;

class DamagingObstacle : public Component {
public:
    DamagingObstacle(GameObject* go) { gameObject = go; }
    void OnCollisionEnter(GameObject* otherObject) override;

};