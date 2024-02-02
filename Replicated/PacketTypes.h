//
// Created by jdhyd on 2/1/2024.
//

#ifndef CSC8503_PACKETTYPES_H
#define CSC8503_PACKETTYPES_H

#include "Quaternion.h"
#include "Vector2.h"
#include "NetworkBase.h"

using namespace NCL;
using namespace Maths;

struct InputPacket : public GamePacket {
    Quaternion playerRotation;
    Vector2 playerDirection;

    InputPacket() {
        type = Received_State;
        size = sizeof(InputPacket) - sizeof(GamePacket);
    }
};

#endif //CSC8503_PACKETTYPES_H
