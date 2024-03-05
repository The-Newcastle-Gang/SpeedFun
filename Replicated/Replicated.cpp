#include "Replicated.h"

Replicated::Replicated() {

}

int Replicated::GetCurrentLevelLen(){
    return levelReader->GetLevelSize();
}


void Replicated::AddBlockToLevel(GameObject *g, GameWorld& world, PrimitiveGameObject* currentPrimitive) {

    world.AddGameObject(g, currentPrimitive->shouldNetwork);
    auto volume = new OBBVolume(currentPrimitive->colliderExtents * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
        .SetOrientation(Quaternion::EulerAnglesToQuaternion(0.0f, 0, 0))
        .SetScale(currentPrimitive->dimensions)
        .SetPosition(currentPrimitive->position);
}


void Replicated::AddSpringToLevel(GameObject* g, GameWorld& world, Vector3 pos) {

    auto currentPrimitive = new PrimitiveGameObject();
    currentPrimitive->position = pos;
    currentPrimitive->colliderExtents = Vector3(4, 0.5f, 4);
    currentPrimitive->dimensions = Vector3(4, 0.5f, 4);

    world.AddGameObject(g, currentPrimitive->shouldNetwork);
    auto volume = new AABBVolume(currentPrimitive->colliderExtents * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
        .SetScale(currentPrimitive->dimensions)
        .SetPosition(currentPrimitive->position);
}

void Replicated::AddTestObjectToLevel(GameObject* g, GameWorld& world, Vector3 size, Vector3 position){

    world.AddGameObject(g, true);
    auto volume = new AABBVolume(size * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
        .SetScale(size)
        .SetPosition(position);
}

void Replicated::AddSwingingBlock(GameObject* g, GameWorld& world) {

    world.AddGameObject(g, true);
    auto volume = new AABBVolume(Vector3(10, 10, 10) * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
        // change these to match the level loader
        .SetScale({5, 5, 5})
        .SetPosition({1, 20, 1});
}

void Replicated::CreatePlayer(GameObject *g, GameWorld& world) {
    constexpr float meshSize = 1.0f;
    world.AddGameObject(g, true);
    auto volume = new CapsuleVolume(meshSize/2, meshSize/2);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
            .SetScale(Vector3(meshSize, meshSize, meshSize))
            .SetPosition(Vector3(0 + (g->GetWorldID()%2) * 10,0,10 * (g->GetWorldID()/2)));
}

void Replicated::AddTriggerVolumeToWorld(Vector3 dimensions, GameObject *g, GameWorld& world){
    world.AddGameObject(g, false);
    auto volume = new AABBVolume(dimensions * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);
    g->GetTransform()
        .SetScale(dimensions);
}