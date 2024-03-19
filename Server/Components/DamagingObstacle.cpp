#include "DamagingObstacle.h"
#include "PlayerRespawner.h"

void DamagingObstacle::OnCollisionEnter(GameObject* otherObject) {
    if (otherObject->GetTag() == Tag::PLAYER) {
        PlayerRespawner* pr;
        if (otherObject->TryGetComponent<PlayerRespawner>(pr)) {
            pr->RespawnPlayer(GetPlayerId(otherObject));
        }
    }
}