//
// Created by c3064585 on 16/02/2024.
//

#ifndef GITIGNORE_TRIGGERVOLUMEOBJECT_H
#define GITIGNORE_TRIGGERVOLUMEOBJECT_H

#include "GameObject.h"
#include <iostream>

using namespace NCL;
using namespace CSC8503;

class TriggerVolumeObject : public GameObject {
public:
    enum class TriggerType {
        Start = 1,
        End = 2,
        Death = 4,
    };

    TriggerVolumeObject(TriggerType triggerEnum) { triggerType = triggerEnum; }

    virtual bool OnCollisionBegin(GameObject* otherObject);
    virtual void OnCollisionEnd(GameObject* otherObject);

private:
    TriggerType triggerType;
    bool startfall;
};


#endif //GITIGNORE_TRIGGERVOLUMEOBJECT_H
