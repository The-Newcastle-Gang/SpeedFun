//
// Created by jdhyd on 2/8/2024.
//

#ifndef CSC8503_CLIENTTHREAD_H
#define CSC8503_CLIENTTHREAD_H

#include "NetworkBase.h"
#include "GameClient.h"
#include "ClientNetworkData.h"

class ClientThread : PacketReceiver {
public:
    ClientThread(GameClient* client, ClientNetworkData* data);
    ~ClientThread();

    void ReadPacketsToSend();
    void Update();
    void ReceivePacket(int type, GamePacket* payload, int source) override;
    void ClearPacketHandlers();

protected:
    GameClient* baseClient;
    ClientNetworkData* networkData;
};


#endif //CSC8503_CLIENTTHREAD_H
