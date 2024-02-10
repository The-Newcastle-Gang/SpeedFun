//
// Created by jdhyd on 2/8/2024.
//

#include "ClientThread.h"
#include "enet.h"

ClientThread::ClientThread(GameClient *client, ClientNetworkData *data) {
    baseClient = client;
    networkData = data;
    baseClient->RegisterPacketHandler(Function, this);
    baseClient->RegisterPacketHandler(Full_State, this);
}


ClientThread::~ClientThread() {
    baseClient->ClearPacketHandlers();
}

void ClientThread::ReadPacketsToSend() {
    while (!networkData->outgoingFunctions.IsEmpty()) {
        FunctionPacket f = networkData->outgoingFunctions.Pop();
        baseClient->SendImportantPacket(f);
    }

    while (!networkData->outgoingInput.IsEmpty()) {
        InputPacket packet = networkData->outgoingInput.Pop();
        baseClient->SendPacket(packet);
    }
}

void ClientThread::ReceivePacket(int type, GamePacket *payload, int source) {
    if (type == Function) {
        auto fp = (FunctionPacket*)payload;
        networkData->incomingFunctions.Push(*fp);
    } else if (type == Full_State) {
        auto fp = (FullPacket*)payload;
        networkData->incomingState.Push(*fp);
    }
}


void ClientThread::Update() {
    ReadPacketsToSend();
    baseClient->UpdateClient();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

