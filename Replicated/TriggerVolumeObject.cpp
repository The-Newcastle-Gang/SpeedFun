//
// Created by c3064585 on 16/02/2024.
//

#include "TriggerVolumeObject.h"
#include "PhysicsObject.h"

using namespace NCL::CSC8503;

void TriggerVolumeObject::OnCollisionBegin(GameObject *otherObject) {
    if(otherObject->GetIsPlayerBool()){
        switch (triggerType) {
            case TriggerType::Start:
                std::cout << "Start volume\n";
                break;
            case TriggerType::End:
                std::cout << "End volume\n";
                break;
            case TriggerType::Death:
                otherObject->GetTransform().SetPosition(otherObject->GetCurrentCheckPointPos());
                break;
            case TriggerType::CheckPoint:
                otherObject->SetCurrentCheckPointPos(this->GetTransform().GetPosition());
                break;

            default:
                    break;
        }
        std::cout << "Player has collided\n";
    }
}

void TriggerVolumeObject::OnCollisionEnd(GameObject *otherObject) {
    if(otherObject->GetIsPlayerBool()){
        std::cout << "Collision with player has ended\n";
    }
}
