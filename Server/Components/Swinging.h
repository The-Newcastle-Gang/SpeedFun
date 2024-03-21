#include "Component.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

class Swinging : public Component {
    enum State {
        RUNNING,WAITING,COOLDOWN
    };

public:
    Swinging(GameObject* go,float period, float cooldown = 0.0f, float waitDelay = 0.0f, float radius = 5.0f, bool changeAxis = false, bool changeDirection = false);
    //void Update(float dt)override;
    void OnCollisionEnter(GameObject* other)override;
    void OnCollisionEnd(GameObject* other)override;
    void Update(float dt)override;

private:

    void UpdateOscillation(float dt);

    Vector3 initPosition;

    float waitDelay = 0.0f;

    float cooldown = 0.0f;

    bool isReturning = false;

    float timer = 0.0f;

    float frequency = 1.0f;

    float radius = 5.0f;

    State state = RUNNING;

    bool changeAxis = false;
    bool changeDirection = false;
};