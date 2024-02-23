
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
/*
* NOTE: If you're applying a big force to an object that touches a spring, use a continuous force spring so it doesn't
*       look like said object is teleporting. Physics sucks.
*/
class Spring : public Component {
public:
    Spring(GameObject* g, Vector3 bI = Vector3(0, 5, 0), float aT = 1.0f, bool c = false, Vector3 cf = Vector3(0,0,0));
    void OnCollisionEnter(const GameObject* otherObject) override;
    void Update(float dt) override;

private:
    Vector3 bounceForce;
    bool continuous = false;
    std::map<const GameObject*, float> pushableObject;
    Vector3 continuousForce; 
    float springActiveTime;
};


#endif //SPEEDFUN_SPRING_H
