#ifndef CSC8503_REPLICATED_H
#define CSC8503_REPLICATED_H


#include "GameWorld.h"
#include "NetworkObject.h"
#include "GameTimer.h"
#include "SafeQueue.h"
#include "LevelReader.h"
#include "enet.h"

#include <vector>
#include <string>

using namespace NCL;
using namespace CSC8503;

class Replicated {
public:
    // In the situation where the client is the remote (Server to client)
    enum RemoteClientCalls {
        AssignPlayer,
        LoadGame,
        Camera_GroundedMove,
        Camera_Jump,
        Camera_Land,
        Camera_Strafe,
	    EndReached,
        Grapple_Event,
        Death_Event,
        Death_Event_End,
        Stage_Start,
        Grapple_Event,
        Player_Velocity_Call
    };


    // In the situation where the server is the remote (Client to server)
    enum RemoteServerCalls {
        StartGame,
        GameLoaded,
        PlayerJump,
        PlayerGrapple,
    };

    Replicated();
    void InitLevel();
    int GetCurrentLevelLen();
    void AddBlockToLevel(GameObject *g, GameWorld& world, PrimitiveGameObject* cur);
    void AddSpringToLevel(GameObject* g, GameWorld& world, Vector3 pos);
    void AddTestObjectToLevel(GameObject *g, GameWorld& world,Vector3 size, Vector3 position);
    void AddTriggerVolumeToWorld(Vector3 dimensions, GameObject *g, GameWorld& world);
    void CreatePlayer(GameObject *g, GameWorld& world);
    void AddSwingingBlock(GameObject* g, GameWorld& world);

    constexpr static int PLAYERCOUNT = 2;
    constexpr static float SERVERHERTZ = 1.0f / 60.0f;
    constexpr static int CHANNELCOUNT = 2;
    constexpr static int BASICPACKETTYPE = 2;
    LevelReader* levelReader;
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