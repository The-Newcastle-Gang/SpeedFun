#include "GameServer.h"
#include "enet.h"

using namespace NCL;
using namespace CSC8503;

GameServer::GameServer(int onPort, int maxClients)	{
    port		= onPort;
    clientMax	= maxClients;
    clientCount = 0;
    netHandle	= nullptr;
    currentSnapshot = 0;



    Initialise();
}

GameServer::~GameServer()	{
    Shutdown();
}

void GameServer::CallRemote(int functionId, FunctionData* data, int peer) {
    // Can later optimize this to adjust packet size based on argument size.
    FunctionPacket packet(functionId, data);
    SendPacket(packet, peer);
}

void GameServer::CallRemoteAll(int functionId, FunctionData* data) {
    FunctionPacket packet(functionId, data);
    SendGlobalPacket(packet);
}

void GameServer::Shutdown() {
    SendGlobalPacket(BasicNetworkMessages::Shutdown);
    enet_host_destroy(netHandle);
    netHandle = nullptr;
}

bool GameServer::Initialise() {
    ENetAddress address = {0};
    address.host = ENET_HOST_ANY;
    address.port = port;

    netHandle = enet_host_create(&address, clientMax, 2, 0, 0);

    if (!netHandle) {
        std::cout << __FUNCTION__ << " failed to create network handle!" << std::endl;
        return false;
    }

    return true;
}

bool GameServer::SendGlobalPacket(int msgID) {
    GamePacket packet;
    packet.type = msgID;
    return SendGlobalPacket(packet);
}

bool GameServer::SendGlobalPacket(GamePacket& packet) {
    ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
    enet_host_broadcast(netHandle, 0, dataPacket);
    UpdateDiagnostics();
    return true;
}

bool GameServer::UpdateDiagnostics() {
    packetsSent.gameTimer->Tick();
    packetsSent.packetCount++;
    auto timeSinceLastPacket = packetsSent.gameTimer->GetTimeDeltaSeconds();
    if (timeSinceLastPacket > packetsSent.maxPacketTime) {
        packetsSent.maxPacketTime = timeSinceLastPacket;
    }

    //std::cout << "Packet sent: " << timeSinceLastPacket << ", ";
    return true;
}


bool GameServer::SendPacket(GamePacket &packet, int peerId) {
    ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
    enet_peer_send(idToPeer[peerId], 0, dataPacket);

    UpdateDiagnostics();

    return true;
}

void GameServer::UpdateServer() {
    if (!netHandle) { return; }
    ENetEvent event;
    while (enet_host_service(netHandle, &event, 0) > 0) {
        int type = event.type;
        ENetPeer* p = event.peer;
        int peer = p->incomingPeerID;

        if (type == ENetEventType::ENET_EVENT_TYPE_CONNECT) {
            std::cout << "Server: New client connected" << std::endl;
            PlayerInfo info{};
            AddPlayerInfo(peer, info);
            idToPeer[peer] = p;
            playerJoined.publish(peer);
        }

        else if (type == ENetEventType::ENET_EVENT_TYPE_DISCONNECT) {
            std::cout << "Server: A client has disconnected" << std::endl;
            idToPeer.erase(peer);
            RemovePlayerInfo(peer);
            playerLeft.publish(peer);
        }

        else if (type == ENetEventType::ENET_EVENT_TYPE_RECEIVE) {
            GamePacket* packet = (GamePacket*)event.packet->data;
            ProcessPacket(packet, peer);
        }

        enet_packet_destroy(event.packet);
    }
}

void GameServer::AddPlayerInfo(int peerId, PlayerInfo &info) {
    players.insert(std::make_pair(peerId, info));
}

void GameServer::RemovePlayerInfo(int peerId) {
    players.erase(peerId);
}


PlayerInfo GameServer::GetPlayerByPeerId(int peerId) {
    return players[peerId];
}

std::unordered_map<int, PlayerInfo> &GameServer::GetPlayerInfo() {
    return players;
}


