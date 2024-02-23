
#ifndef SPEEDFUN_SPRING_H
#define SPEEDFUN_SPRING_H

#include "Component.h"
#include "Vector3.h"
#include "Vector2.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"

#include <functional>

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

class Spring : public Component {
public:
    Spring(GameObject* g, Vector3 bI = Vector3(0, 5, 0), bool c = false, float cftl = 1.0f, Vector3 cf = Vector3(0,0,0));
    void OnCollisionEnter(const GameObject* otherObject) override;
    void Update(float dt) override;

private:
    Vector3 bounceForce;
    float cooldown;
    float cooldownLimit = 1.0f;

    bool continuous = false;
    std::map<const GameObject*, float> pushableObject;
    Vector3 continuousForce;
    float continuousForceTimerLimit;
};


#endif //SPEEDFUN_SPRING_H
