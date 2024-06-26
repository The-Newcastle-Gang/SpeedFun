//
// Created by jdhyd on 2/8/2024.
//

#ifndef CSC8503_SERVERTHREAD_H
#define CSC8503_SERVERTHREAD_H

#include "NetworkBase.h"
#include "GameServer.h"
#include "ServerNetworkData.h"

class ServerThread : PacketReceiver {
public:
    ServerThread(GameServer* server, ServerNetworkData* data);
    ~ServerThread();

    void ReadPacketsToSend();
    void Update();
    void ReceivePacket(int type, GamePacket* payload, int source) override;
private:
    GameServer* baseServer;
    ServerNetworkData* networkData;

};


#endif //CSC8503_SERVERTHREAD_H
