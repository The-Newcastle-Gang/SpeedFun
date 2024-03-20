#pragma once
#include "Component.h"
#include "Vector3.h"
#include "Vector2.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"
#include "Replicated.h"

#include <functional>

class PlayerRespawner : public Component {
public:
    PlayerRespawner(GameObject* go, std::function<void(int)> deathFunc) {
        gameObject = go;
        deathTriggerFunction = deathFunc; //this was a bit of a lazy way to get around the way triggers were made but its fine
    }
    void RespawnPlayer(int id) {
        gameObject->GetPhysicsObject()->ClearForces();
        gameObject->GetPhysicsObject()->ClearVelocity();
        gameObject->GetTransform().SetPosition(gameObject->GetCurrentCheckPointPos());

        deathTriggerFunction(id);
    }

private:
    std::function<void(int)> deathTriggerFunction;
};