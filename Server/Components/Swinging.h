#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include <unordered_map>

using namespace NCL;
using namespace CSC8503;

class Swinging : public Component {
    enum State {
        RUNNING,WAITING,COOLDOWN
    };

public:
    Swinging(GameObject* go,float period,float distance,Vector3 direction, float cooldown = 0.0f, float waitDelay = 0.0f);
    //void Update(float dt)override;
    void OnCollisionEnter(GameObject* other)override;
    void OnCollisionEnd(GameObject* other)override;
    void PhysicsUpdate(float dt)override;

private:

    void UpdateOscillation(float dt);

    PhysicsObject* phys;
    Vector3 initPosition;
    Vector3 lastVelocity;

    Vector3 normalisedDirection; //should be normalised!
    float distance = 1.0f;

    float waitDelay = 0.0f;

    float cooldown = 0.0f;

    bool isReturning = false;

    float timer = 0.0f;

    float frequency = 1.0f;

    State state = RUNNING;

    std::unordered_map<GameObject*,bool> objectsOnPlatform; //means we only add to the map once per player.
};