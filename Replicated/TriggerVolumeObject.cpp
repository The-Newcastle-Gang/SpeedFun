#include "TriggerVolumeObject.h"

using namespace NCL::CSC8503;

TriggerVolumeObject::TriggerVolumeObject(TriggerVolumeObject::TriggerType triggerEnum, std::function<int(GameObject*)> idGetter) : TriggerSink(triggerSignal),
                                                                                         TriggerSinkEndVol(triggerSignalEndVol) {
    triggerType = triggerEnum;
    GetPlayerId = idGetter;
}

void TriggerVolumeObject::OnCollisionBegin(GameObject *otherObject) {
    if(otherObject->GetTag() == Tag::PLAYER){
        switch (triggerType) {
            case TriggerType::Start:
                std::cout << "Start volume\n";
                triggerSignal.publish(GetPlayerId(otherObject));
                break;
            case TriggerType::End:
                std::cout << "End volume\n";
                triggerSignalEndVol.publish(GetPlayerId(otherObject));
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
                if(otherObject->GetCurrentCheckPointPos() != Vector3()) { return; }
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
