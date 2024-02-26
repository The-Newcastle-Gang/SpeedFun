#include "SwingingObject.h"

using namespace NCL;
using namespace CSC8503;


void SwingingObject::Update(float dt)
{
    timer += dt;

    Vector3 tempPos = startingPos;
    tempPos.y = startingPos.y + sin(timer) * radius;
    tempPos.z = startingPos.y + cos(timer) * radius;
    gameObject->GetTransform().SetPosition(tempPos);
}
