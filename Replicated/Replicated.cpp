//
// Created by c3042750 on 30/01/2024.
//

#include "Replicated.h"



Replicated::Replicated() {
    networkIdCounter = 0;
}

void Replicated::CreateObject(GameObject *g, GameWorld& world) {

}

void Replicated::CreatePlayer(GameObject *g) {
    constexpr float meshSize = 1.0f;
    auto volume = new AABBVolume(Vector3(meshSize, meshSize, meshSize));
    g->SetBoundingVolume((CollisionVolume*)volume);
    g->SetNetworkObject(new NetworkObject(*g, networkIdCounter++));
    networkObjects.push_back(g->GetNetworkObject());
    g->GetTransform()
            .SetScale(Vector3(meshSize, meshSize, meshSize))
            .SetPosition(Vector3(0 + (networkIdCounter%2) * 10,0,10 * (networkIdCounter/2)));
}


