#include "Component.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

class ObjectOscillator : public Component {
    enum State {
        RUNNING,WAITING,COOLDOWN
    };

public:
    ObjectOscillator(GameObject* go,float period,float distance,Vector3 direction, float cooldown = 0.0f, float waitDelay = 0.0f);
    void Update(float dt)override;

private:

    void UpdateOscillation();
    Vector3 initPosition;

    Vector3 normalisedDirection; //should be normalised!
    float distance = 1.0f;

    float waitDelay = 0.0f;

    float cooldown = 0.0f;

    bool isReturning = false;

    float timer = 0.0f;

    float frequency = 1.0f;

    State state = RUNNING;

};