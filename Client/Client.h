//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_CLIENT_H
#define CSC8503_CLIENT_H

#include "NetworkBase.h"
#include "GameClient.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Replicated.h"
#include "PacketTypes.h"
#include "Quaternion.h"
#include "RenderObject.h"
#include "StateMachine.h"
#include "State.h"
#include "GameplayState.h"
#include "LoadingState.h"
#include "MenuState.h"

#include <iostream>
#include <memory>
#include <array>

using namespace NCL;
using namespace CSC8503;

class Client : public PacketReceiver {
public:
    Client();
    void InitClient();
    std::string GetAddress();
    void Update(float dt);
    void ReceivePacket(int type, GamePacket* payload, int source) override;

private:
    std::unique_ptr<StateMachine> stateManager;
    std::unique_ptr<GameClient> baseClient;
    std::unique_ptr<GameWorld> world;
    std::unique_ptr<GameTechRenderer> renderer;
    std::unique_ptr<Replicated> replicated;

    GameObject* thisPlayer;

    void InitNetworking();
    void InitGame();
    void SendInputData();
    void InitCamera();
    void InitialiseAssets();
    void RegisterPackets();
    void CreatePlayers();

    ShaderBase *GetShader(const string &shader);
    MeshGeometry* GetMesh(const std::string& name);

    std::unordered_map<std::string, MeshGeometry*> meshes;
    std::unordered_map<std::string, ShaderBase*> shaders;

    void AssignPlayer(unsigned char *data);

    void TemporaryLevelLoad();

    void InitStateManager();
};


#endif //CSC8503_CLIENT_H
