#include "TriggerVolumeObject.h"
#include "Components/PlayerRespawner.h"

using namespace NCL::CSC8503;

TriggerVolumeObject::TriggerVolumeObject(TriggerVolumeObject::TriggerType triggerEnum, std::function<int(GameObject*)> idGetter) : TriggerSink(triggerSignal),
                                                                                         TriggerSinkEndVol(triggerSignalEndVol),
                                                                                         TriggerSinkDeathVol(triggerSignalDeathVol),
                                                                                         TriggerSinkDeathVolEnd(triggerSignalDeathVolEnd),
                                                                                         TriggerSinkStartVol(triggerSignalStartVol){
    triggerType = triggerEnum;
    GetPlayerId = std::move(idGetter);
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
                otherObject->SetCurrentCheckPointPos(this->GetTransform().GetPosition());

                break;

            case TriggerType::Death:
                PlayerRespawner* pr;
                if (otherObject->TryGetComponent<PlayerRespawner>(pr)) {
                    pr->RespawnPlayer(GetPlayerId(otherObject));
                }
                //triggerSignalDeathVol.publish(GetPlayerId(otherObject));

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
                triggerSignalStartVol.publish(GetPlayerId(otherObject));
                break;
            case TriggerType::End:
                break;
            case TriggerType::Death:
                // Call signal to push main canvas pack or pop current one off the stack
                triggerSignalDeathVolEnd.publish(GetPlayerId(otherObject));

                break;
            case TriggerType::CheckPoint:
                break;
            default:
                break;
        }
    }
}
