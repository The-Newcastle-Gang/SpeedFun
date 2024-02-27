#include "DamagingObstacle.h"

void DamagingObstacle::OnCollisionEnter(GameObject* otherObject) {
    if (otherObject->GetTag() == Tag::PLAYER) {
        //respawn the player here
        std::cout << "PLAYER HURT!!"<<"\n";
    }
}