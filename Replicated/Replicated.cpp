//
// Created by c3042750 on 30/01/2024.
//

#include "Replicated.h"



Replicated::Replicated() {

}

int Replicated::GetCurrentLevelLen(){
    return levelReader->GetLevelSize();
}


void Replicated::AddBlockToLevel(GameObject *g, GameWorld& world, PrimitiveGameObject* currentPrimitive) {

    world.AddGameObject(g, currentPrimitive->shouldNetwork);
    auto volume = new AABBVolume(currentPrimitive->colliderExtents * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
        .SetScale(currentPrimitive->dimensions)
        .SetPosition(currentPrimitive->position);
}

void Replicated::AddSwingingBlock(GameObject* g, GameWorld& world) {

    world.AddGameObject(g, true);
    auto volume = new AABBVolume(Vector3(10, 10, 10) * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
        .SetScale({10, 10, 10})
        .SetPosition({1, 20, 1});
}

void Replicated::CreatePlayer(GameObject *g, GameWorld& world) {
    constexpr float meshSize = 1.0f;
    world.AddGameObject(g, true);
    auto volume = new AABBVolume(Vector3(meshSize, meshSize, meshSize));
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
            .SetScale(Vector3(meshSize, meshSize, meshSize))
            .SetPosition(Vector3(0 + (g->GetWorldID()%2) * 10,0,10 * (g->GetWorldID()/2)));

}

void Replicated::AddTriggerVolumeToWorld(Vector3 dimensions, GameObject *g, GameWorld& world){
    constexpr float meshSize = 1.0f;
    world.AddGameObject(g, false);
    auto volume = new AABBVolume(dimensions * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);
    g->GetTransform()
        .SetScale(Vector3(meshSize, meshSize, meshSize));
}