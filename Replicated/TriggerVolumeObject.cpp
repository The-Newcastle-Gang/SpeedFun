#include "TriggerVolumeObject.h"

using namespace NCL::CSC8503;

void TriggerVolumeObject::OnCollisionBegin(GameObject *otherObject) {
    if(otherObject->GetTag() == Tag::PLAYER){
        switch (triggerType) {
            case TriggerType::Start:
                std::cout << "Start volume\n";
                break;
            case TriggerType::End:
                std::cout << "End volume\n";
                break;
            case TriggerType::Death:
                otherObject->GetPhysicsObject()->ClearForces();
                otherObject->GetPhysicsObject()->ClearVelocity();
                otherObject->GetTransform().SetPosition(otherObject->GetCurrentCheckPointPos());
                //revert camera, function in gameplay state
                break;
            case TriggerType::CheckPoint:
                otherObject->SetCurrentCheckPointPos(this->GetTransform().GetPosition());
                break;

            default:
                    break;
        }
    }
}

void TriggerVolumeObject::OnCollisionEnd(GameObject *otherObject) {
    if(otherObject->GetTag() == Tag::PLAYER){

        std::cout << "Collision with player has ended\n";
        switch (triggerType) {
            case TriggerType::Start:
                otherObject->SetCurrentCheckPointPos(this->GetTransform().GetPosition());
                break;
            case TriggerType::End:
                break;
            case TriggerType::Death:
                break;
            case TriggerType::CheckPoint:
                break;
            default:
                break;
        }
    }
}
