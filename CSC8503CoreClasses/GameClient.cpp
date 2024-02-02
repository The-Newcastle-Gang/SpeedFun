#include "GameClient.h"
#include "enet.h"

using namespace NCL;
using namespace CSC8503;

GameClient::GameClient()	{
    netPeer = {0};
    netHandle = enet_host_create(nullptr, 1, 1, 0, 0);
    lastServerSnapshot = 0;
}

GameClient::~GameClient()	{
    enet_host_destroy(netHandle);
    netHandle = nullptr;
}

bool GameClient::Connect(std::string ip, int portNum) {
    ENetAddress address = {0};
    enet_address_set_host(&address, ip.c_str());
    address.port = portNum;

    netPeer = enet_host_connect(netHandle, &address, 2, 0);
    return netPeer != nullptr;
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
        } else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            GamePacket *packet = (GamePacket*)event.packet->data;
            ProcessPacket(packet);
        }
        enet_packet_destroy(event.packet);
    }
}

void GameClient::SendPacket(GamePacket& payload) {
    ENetPacket *dataPacket = enet_packet_create(&payload, payload.GetTotalSize(), 0);
    enet_peer_send(netPeer, 0, dataPacket);
}

void GameClient::SendPacket(GamePacket& payload, int something) {
    SendPacket(payload);
}