//
// Created by idhant on 14-02-2024.
//

#include "GrappleComponent.h"
using namespace NCL::CSC8503;

GrappleComponent::GrappleComponent(GameObject* go, GameWorld* pWorld) : world(pWorld) {

    world = pWorld;
    gameObject = go;

}

void GrappleComponent::PhysicsUpdate(float dt) {

}

void GrappleComponent::ProcessMovementInput(Vector3 position) {
    std::cout << position;
}
