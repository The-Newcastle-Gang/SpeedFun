#include "SwingingObject.h"

using namespace NCL;
using namespace CSC8503;


void SwingingObject::Update(float dt)
{
    ascending ? timer += dt : timer -= dt;


    Vector3 tempPos = startingPos;
    tempPos.y = startingPos.y + sin(timer) * radius;
    tempPos.z = startingPos.z + cos(timer) * radius;
    gameObject->GetTransform().SetPosition(tempPos);

    CheckTimerState();
}

void SwingingObject::CheckTimerState()
{
    if (timer >= MAX_TIMER) { ascending = false; }
    if (timer <= MIN_TIMER) { ascending = true; }
}

void SwingingObject::OnCollisionEnter(GameObject *otherObject) {
    Component::OnCollisionEnter(otherObject);
    std::cout << "player has collided with swinging object\n";
}

void SwingingObject::OnCollisionStay(NCL::CSC8503::GameObject *otherObject) {
    Component::OnCollisionEnter(otherObject);
    std::cout << "player has collided with swinging object\n";
}

void SwingingObject::OnCollisionEnd(GameObject *otherObject) {
    Component::OnCollisionEnter(otherObject);
    std::cout << "player has STOPPED colliding with swinging object\n";
}
