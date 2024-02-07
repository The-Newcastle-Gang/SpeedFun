//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_REPLICATED_H
#define CSC8503_REPLICATED_H


#include "GameWorld.h"
#include "NetworkObject.h"
#include "GameTimer.h"
#include <vector>
#include <string>

using namespace NCL;
using namespace CSC8503;

class Replicated {
public:
    enum RemoteClientCalls {
        AssignPlayer,
        LoadGame,
    };

    enum RemoteServerCalls {
        StartGame = 40,
        GameLoaded = 41,
    };

    Replicated();
    void CreateObject(GameObject *g, GameWorld& world);
    void CreatePlayer(GameObject *g, GameWorld& world);
    constexpr static int PLAYERCOUNT = 2;
    constexpr static float SERVERHERTZ = 1.0f / 60.0f;
    constexpr static int CHANNELCOUNT = 2;
    constexpr static int BASICPACKETTYPE = 2;

};

struct Diagnostics {
    float averagePacketTime;
    float maxPacketTime;
    float minPacketTime;
    float packetCount;
    GameTimer* gameTimer;

    Diagnostics() {
        gameTimer = new GameTimer();
    }

    ~Diagnostics() {
        delete gameTimer;
    }
};


#endif //CSC8503_REPLICATED_H
