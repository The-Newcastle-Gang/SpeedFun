#include "GameClient.h"
#include "enet.h"

using namespace NCL;
using namespace CSC8503;

GameClient::GameClient() : OnServerConnected(serverConnected) {
    netPeer = { 0 };
    netHandle = enet_host_create(nullptr, 1, 1, 0, 0);
    lastServerSnapshot = 0;
}

GameClient::~GameClient()	{
    enet_host_destroy(netHandle);
    netHandle = nullptr;
}

bool GameClient::Connect(const std::string& ip, int portNum) {
    ENetAddress address = {0};
    enet_address_set_host(&address, ip.c_str());
    address.port = portNum;

    netPeer = enet_host_connect(netHandle, &address, Replicated::CHANNELCOUNT, 0);
    return netPeer != nullptr;
}

void GameClient::RemoteFunction(int functionId, FunctionData* data) {
    // Can later optimize this to adjust packet size based on argument size.
    FunctionPacket packet(functionId, data);
    SendPacket(packet);
}

void GameClient::UpdateDiagnostics(Diagnostics& d) {
    d.gameTimer->Tick();
    d.packetCount++;
    auto timeSinceLastPacket = d.gameTimer->GetTimeDeltaSeconds();
    if (timeSinceLastPacket > 0.1) {
        std::cout << "Delay in packets recieved: " << timeSinceLastPacket << std::endl;
    }
}


void GameClient::Disconnect() {
    enet_peer_disconnect_now(netPeer, 0);
}


void GameClient::UpdateClient() {


    if (netHandle == nullptr) {
        return;
    }

    updateCalled.gameTimer->Tick();
    if (updateCalled.gameTimer->GetTimeDeltaSeconds() > 0.1) {
        std::cout << "Delay in update client function: " << updateCalled.gameTimer->GetTimeDeltaSeconds() << std::endl;
    }

    ENetEvent event;
    while (enet_host_service(netHandle, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_CONNECT) {
            std::cout << "Connected to server!" << std::endl;
            serverConnected.publish();
        } else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            auto packet = (GamePacket*)event.packet->data;
            UpdateDiagnostics(packetsReceived);
            ProcessPacket(packet);
        }
        enet_packet_destroy(event.packet);
    }
}

void GameClient::SendPacket(GamePacket& payload) {
    ENetPacket *dataPacket = enet_packet_create(&payload, payload.GetTotalSize(), Replicated::BASICPACKETTYPE);
    enet_peer_send(netPeer, 0, dataPacket);
}