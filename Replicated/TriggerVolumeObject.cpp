//
// Created by c3064585 on 16/02/2024.
//

#include "TriggerVolumeObject.h"


bool TriggerVolumeObject::OnCollisionBegin(GameObject *otherObject) {
    if(otherObject->GetIsPlayerBool()){
        switch (triggerType) {
            case TriggerType::Start: // Start volume
                std::cout << "Start volume\n";
                break;
            case TriggerType::End: // End Volume
                std::cout << "End volume\n";
                break;
            case TriggerType::Death: // Death Volume
                std::cout << "Death volume\n";
                break;

            default:
                    break;
        }
        std::cout << "Player has collided\n";
        startfall = true;
        std::cout << startfall << std::endl;
        return startfall;
    }
}

void TriggerVolumeObject::OnCollisionEnd(GameObject *otherObject) {
    if(otherObject->GetIsPlayerBool()){
        std::cout << "Collision with player has ended\n";
    }
}
