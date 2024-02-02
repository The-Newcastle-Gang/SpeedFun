//
// Created by c3042750 on 30/01/2024.
//

#include "Replicated.h"

static int NetworkIds = 0;

void Replicated::CreateObject(GameObject *g, GameWorld& world) {

}

void Replicated::CreatePlayer(GameObject *g, GameWorld& world) {
    constexpr float meshSize = 1.0f;
    auto volume = new AABBVolume(Vector3(meshSize, meshSize, meshSize));
    g->SetBoundingVolume((CollisionVolume*)volume);
    g->GetTransform()
        .SetScale(Vector3(meshSize, meshSize, meshSize))
        .SetPosition(Vector3(0,0,0));

    g->SetNetworkObject(new NetworkObject(*g, NetworkIds++));
}
