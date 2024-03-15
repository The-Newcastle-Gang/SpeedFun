//
// Created by c3042750 on 30/01/2024.
//

#include "Replicated.h"

const Vector4 Replicated::PLATINUM = Vector4(0.941f, 0.918f, 0.839f, 1.0f);
const Vector4 Replicated::GOLD = Vector4(0.788f, 0.69f, 0.216f, 1.0f);
const Vector4 Replicated::SILVER = Vector4(0.843f, 0.843f, 0.843f, 1.0f);
const Vector4 Replicated::BRONZE = Vector4(0.416f, 0.22f, 0.02f, 1.0f);

Replicated::Replicated() {

}

int Replicated::GetCurrentLevelLen(){
    return levelReader->GetLevelSize();
}


void Replicated::AddBlockToLevel(GameObject *g, GameWorld& world, PrimitiveGameObject* currentPrimitive) {

    world.AddGameObject(g, currentPrimitive->shouldNetwork);
    auto volume = new OBBVolume(currentPrimitive->colliderExtents * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);
    Vector3 tempFix = (currentPrimitive->rotation).Quaternion::ToEuler();
    tempFix *= Vector3(-1,-1,1);

    g->GetTransform()
        .SetPosition(currentPrimitive->position)
        .SetScale(currentPrimitive->dimensions)
        .SetOrientation(Quaternion::EulerAnglesToQuaternion(tempFix.x, tempFix.y, tempFix.z));
 
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
    auto volume = new CapsuleVolume(meshSize * 0.5f, meshSize * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);

    g->GetTransform()
            .SetScale(Vector3(meshSize, meshSize, meshSize))
            .SetPosition(Vector3(0 + (g->GetWorldID()%2) * 10,0,10 * (g->GetWorldID()/2)));
}

void Replicated::AddGrapplesToWorld(GameObject *g, GameWorld &world, int index) {
    constexpr float meshSize = 1.0f;
    world.AddGameObject(g, true);
    auto volume = new SphereVolume(meshSize * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);
    g->GetTransform()
        .SetScale(Vector3(meshSize, meshSize, meshSize))
        .SetPosition(Vector3(index * 10, 10, 10));

    g->SetTag(Tag::GRAPPLE);

}

void Replicated::AddTriggerVolumeToWorld(Vector3 dimensions, GameObject *g, GameWorld& world){
    world.AddGameObject(g, false);
    auto volume = new AABBVolume(dimensions * 0.5f);
    g->SetBoundingVolume((CollisionVolume*)volume);
    g->GetTransform()
        .SetScale(dimensions);
}