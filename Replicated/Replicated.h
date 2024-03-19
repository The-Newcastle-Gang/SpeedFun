//
// Created by c3042750 on 30/01/2024.
//

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
        Player_Animation_Call,
        Death_Event,
        Death_Event_End,
        Stage_Start,
        Player_Velocity_Call,
        GameInfo_Timer,
        GameInfo_GrappleAvailable,
        GameInfo_PlayerPositions,
        SetNetworkActive,
        ToggleGrapple,
};


    // In the situation where the server is the remote (Client to server)
    enum RemoteServerCalls {
        StartGame,
        GameLoaded,
        PlayerJump,
        PlayerGrapple,
        PlayerDebug
    };


    static constexpr Vector4 PLATINUM = Vector4(0.941f, 0.918f, 0.839f, 1.0f);
    static constexpr Vector4 GOLD = Vector4(0.788f, 0.69f, 0.216f, 1.0f);
    static constexpr Vector4 SILVER = Vector4(0.843f, 0.843f, 0.843f, 1.0f);
    static constexpr Vector4 BRONZE = Vector4(0.416f, 0.22f, 0.02f, 1.0f);


    enum PlayerAnimationStates {
        RUNNING_FORWARD,
        RUNNING_BACK,
        RUNNING_LEFT,
        RUNNING_RIGHT,
        FALLING,
        JUMP,
        IDLE
    };

    struct RemoteAnimationData
    {
        int networkID = 0;
        PlayerAnimationStates state = IDLE;
    };


    Replicated();
    void InitLevel();
    int GetCurrentLevelLen();
    void AddBlockToLevel(GameObject *g, GameWorld& world, PrimitiveGameObject* cur);
    void AddSpringToLevel(GameObject* g, GameWorld& world, Vector3 pos);
    void AddTestObjectToLevel(GameObject *g, GameWorld& world,Vector3 size, Vector3 position, bool shouldNetwork);
    void AddTriggerVolumeToWorld(Vector3 dimensions, GameObject *g, GameWorld& world);
    void CreatePlayer(GameObject *g, GameWorld& world);

    constexpr static int PLAYERCOUNT = 2;
    constexpr static float SERVERHERTZ = 1.0f / 60.0f;
    constexpr static int CHANNELCOUNT = 2;
    constexpr static int BASICPACKETTYPE = 2;
    constexpr static float GRAPPLEDISTANCE = 50.0f;
    constexpr static Vector3 HANDOFFSET = {0.0, -0.2, 0.0};
    LevelReader* levelReader;

    void AddGrapplesToWorld(GameObject *g, GameWorld &world, int index);
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