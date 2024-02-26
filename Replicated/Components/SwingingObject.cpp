#include "SwingingObject.h"

using namespace NCL;
using namespace CSC8503;


void SwingingObject::Update(float dt)
{
    ascending ? timer += dt : timer -= dt;

    Vector3 tempPos = startingPos;
    tempPos.y = startingPos.y + sin(timer) * radius;
    tempPos.z = startingPos.y + cos(timer) * radius;
    gameObject->GetTransform().SetPosition(tempPos);


    std::cout << tempPos.y << std::endl;

    CheckTimerState();

}

void SwingingObject::CheckTimerState()
{
    if (timer >= MAX_TIMER) { ascending = false; }
    if (timer <= MIN_TIMER) { ascending = true; }
}
