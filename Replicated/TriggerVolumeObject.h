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

    virtual void OnCollisionBegin(GameObject* otherObject);
    virtual void OnCollisionEnd(GameObject* otherObject);

private:

};


#endif //GITIGNORE_TRIGGERVOLUMEOBJECT_H
