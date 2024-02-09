//
// Created by jdhyd on 2/8/2024.
//

#include "ServerThread.h"

ServerThread::ServerThread(GameServer *server, ServerNetworkData *data) {
    baseServer = server;
    networkData = data;
    baseServer->RegisterPacketHandler(Function, this);
    baseServer->RegisterPacketHandler(Received_State, this);
}

ServerThread::~ServerThread() {
    baseServer->ClearPacketHandlers();
}

void ServerThread::ReadPacketsToSend() {
    while (!networkData->outgoingGlobalFunctions.IsEmpty()) {
        FunctionPacket f = networkData->outgoingGlobalFunctions.Pop();
        baseServer->SendGlobalImportantPacket(f);
    }

    while (!networkData->outgoingFunctions.IsEmpty()) {
        std::pair<int, FunctionPacket> data = networkData->outgoingFunctions.Pop();
        baseServer->SendImportantPacket(data.second, data.first);
    }

    while (!networkData->outgoingState.IsEmpty()) {
        FullPacket packet = networkData->outgoingState.Pop();
        baseServer->SendGlobalPacket(packet);
    }
}

void ServerThread::ReceivePacket(int type, GamePacket *payload, int source) {
    if (type == Function) {
        auto fp = (FunctionPacket*)payload;
        networkData->incomingFunctions.Push(std::make_pair(source, *fp));
    } else if (type == Received_State) {
        auto rp = (InputPacket*)payload;
        networkData->incomingInput.Push(std::make_pair(source, *rp));
    }
}

void ServerThread::Update() {
    ReadPacketsToSend();
    baseServer->UpdateServer();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
