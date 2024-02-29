//
// Created by c3064585 on 16/02/2024.
//

#ifndef GITIGNORE_TRIGGERVOLUMEOBJECT_H
#define GITIGNORE_TRIGGERVOLUMEOBJECT_H

#include "GameObject.h"
#include "PhysicsObject.h"
#include "Vector4.h"
#include "entt.hpp"

using namespace NCL;
using namespace CSC8503;

typedef entt::sigh<void()> TriggerSignal;

class TriggerVolumeObject : public GameObject {
public:
    enum class TriggerType {
        Start = 1,
        End = 2,
        Death = 4,
        CheckPoint = 8,
    };

    TriggerVolumeObject(TriggerType triggerEnum);

    virtual void OnCollisionBegin(GameObject* otherObject);
    virtual void OnCollisionEnd(GameObject* otherObject);

    entt::sink<TriggerSignal> TriggerSink;

private:
    TriggerType triggerType;
    TriggerSignal triggerSignal;
};

#endif //GITIGNORE_TRIGGERVOLUMEOBJECT_H
