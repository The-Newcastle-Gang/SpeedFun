#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "AnimatorObject.h"
#include "Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()	{
    mainCamera = new Camera();

    shuffleConstraints	= false;
    shuffleObjects		= false;
    worldIDCounter		= 0;
    worldStateCounter	= 0;
    networkIdCounter    = 0;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
    gameObjects.clear();
    constraints.clear();
    pointLights.clear();
    networkObjects.clear();
    worldIDCounter		= 0;
    worldStateCounter	= 0;
    networkIdCounter    = 0;
}

void GameWorld::ClearAndErase() {
    for (auto& i : gameObjects) {
        delete i;
        i = nullptr;
    }
    for (auto& i : constraints) {
        delete i;
        i = nullptr;
    }
    Clear();
}

void GameWorld::AddGameObject(GameObject* o, bool isNetworked) {
    gameObjects.emplace_back(o);
    o->SetWorldID(worldIDCounter++);
    worldStateCounter++;

    if (isNetworked) {
        o->SetNetworkObject(new NetworkObject(*o, networkIdCounter++));
        networkObjects.push_back(o->GetNetworkObject());
    }
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
    gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
    if (andDelete) {
        delete o;
        o = nullptr;
    }
    worldStateCounter++;
}

void GameWorld::GetObjectIterators(
    GameObjectIterator& first,
    GameObjectIterator& last) const {

    first	= gameObjects.begin();
    last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
    for (GameObject* g : gameObjects) {
        f(g);
    }
}

void GameWorld::UpdateWorld(float dt) {
    for (GameObject* g : gameObjects) {
        if (g->GetAnimatorObject()) {
            g->GetAnimatorObject()->UpdateAnimation(dt);
        }
    }

	for (GameObject* gameObject : gameObjects)gameObject->Update(dt);

	auto rng = std::default_random_engine{};
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);


    //if (shuffleObjects) {
    //    std::shuffle(gameObjects.begin(), gameObjects.end(), e);
    //}

    //if (shuffleConstraints) {
    //    std::shuffle(constraints.begin(), constraints.end(), e);
    //}
}

void GameWorld::UpdateWorldPhysics(float dt) {
	for (GameObject* gameObject : gameObjects)gameObject->PhysicsUpdate(dt);
}

void GameWorld::StartWorld() {
	for (GameObject* gameObject : gameObjects)gameObject->Start();
    if (gameObjects.size() > 2000) {
        std::cout << "CLIENT: " << gameObjects.size() - 2000 << "\n";
    }
    else {
        std::cout << "SERVER: " << gameObjects.size()<< "\n";

    }
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, GameObject* ignoreThis,int layerMask) const {
    //The simplest raycast just goes through each object and sees if there's a collision
    RayCollision collision;

    for (auto& i : gameObjects) {
        if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
            continue;
        }
        if (i == ignoreThis) {
            continue;
        }

        if (!i->GetPhysicsObject()) continue;

        if (!(i->GetPhysicsObject()->GetLayer() & layerMask))continue;

        RayCollision thisCollision;
        if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {

            if (!closestObject) {
                closestCollision		= collision;
                closestCollision.node = i;
                return true;
            }
            else {
                if (thisCollision.rayDistance < collision.rayDistance) {
                    thisCollision.node = i;
                    collision = thisCollision;
                }
            }
        }
    }
    if (collision.node) {
        closestCollision		= collision;
        closestCollision.node	= collision.node;
        return true;
    }
    return false;
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
    constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
    constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
    if (andDelete) {
        delete c;
        c = nullptr;
    }
}

void GameWorld::GetConstraintIterators(
    std::vector<Constraint*>::const_iterator& first,
    std::vector<Constraint*>::const_iterator& last) const {
    first	= constraints.begin();
    last	= constraints.end();
}