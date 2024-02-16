//
// Created by c3064585 on 16/02/2024.
//

#include "TriggerVolumeObject.h"


void TriggerVolumeObject::OnCollisionBegin(GameObject *otherObject) {
    if(otherObject->GetIsPlayerBool()){
        std::cout << "Player has collided\n";
    }
}

void TriggerVolumeObject::OnCollisionEnd(GameObject *otherObject) {
    if(otherObject->GetIsPlayerBool()){
        std::cout << "Collision with player has ended\n";
    }
}
