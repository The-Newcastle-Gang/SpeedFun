#pragma once

#include "Component.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

class DamagingObstacle : public Component {
public:
    DamagingObstacle(GameObject* go, std::function<int(GameObject*)> idGetter) { gameObject = go; GetPlayerId = idGetter; }
    void OnCollisionEnter(GameObject* otherObject) override;

private:
    std::function<int(GameObject*)> GetPlayerId;
};