//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_REPLICATED_H
#define CSC8503_REPLICATED_H


#include "GameWorld.h"
#include "NetworkObject.h"
#include <string>

using namespace NCL;
using namespace CSC8503;

class Replicated {
public:
    static void CreateObject(GameObject* g, GameWorld& world);
    static void CreatePlayer(GameObject *g, GameWorld &world);

    static int NetworkIds;
};


#endif //CSC8503_REPLICATED_H
