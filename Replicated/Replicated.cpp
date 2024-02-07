//
// Created by c3042750 on 30/01/2024.
//

#include "Replicated.h"



Replicated::Replicated() {
}

void Replicated::CreateLevel(GameWorld& world){

    auto lr = new LevelReader();
    if(!lr->HasReadLevel("finaltest.json")){
        return;
    }
    auto pl = lr->GetPrimitiveList();

    for(auto x: pl){
        
    }

}

void Replicated::CreateObject(GameObject *g, GameWorld& world, Vector3 position) {

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


