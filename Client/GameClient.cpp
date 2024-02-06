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

    netPeer = enet_host_connect(netHandle, &address, 2, 0);
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
}


void GameClient::Disconnect() {
    enet_peer_disconnect_now(netPeer, 0);
}


void GameClient::UpdateClient() {
    if (netHandle == nullptr) {
        return;
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
    ENetPacket *dataPacket = enet_packet_create(&payload, payload.GetTotalSize(), 0);
    enet_peer_send(netPeer, 0, dataPacket);
}