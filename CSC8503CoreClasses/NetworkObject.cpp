#include "NetworkObject.h"
#include "enet.h"
#include <chrono>
using namespace NCL;
using namespace CSC8503;

NetworkObject::NetworkObject(GameObject& o, int id) : object(o)	{
	deltaErrors = 0;
	fullErrors  = 0;
	networkID   = id;
}

NetworkObject::~NetworkObject()	{
}

bool NetworkObject::ReadPacket(GamePacket& p) {
    if (p.type == Delta_State) {
        return ReadDeltaPacket((DeltaPacket&)p);
    }

    if (p.type == Full_State) {
        return ReadFullPacket((FullPacket&)p);
    }
    return false;
}

bool NetworkObject::WritePacket(FullPacket& packet, bool deltaFrame, int stateID) {
	return WriteFullPacket(packet);
}

//Client objects recieve these packets
bool NetworkObject::ReadDeltaPacket(DeltaPacket &p) {
	return true;
}

bool NetworkObject::ReadFullPacket(FullPacket &p) {
    if (p.fullState.stateID < lastFullState.stateID) {
        std::cout << "Packet recieved out of order." << std::endl;
        return false;
    }

    lastFullState = p.fullState;

    if (object.GetTransform().GetPosition() != lastFullState.position) {
        std::cout << "Position updated on client: " << std::chrono::system_clock::now() << std::endl;
    }

    object.GetTransform().SetPosition(lastFullState.position);
    object.GetTransform().SetOrientation(lastFullState.orientation);

    return true;
}

bool NetworkObject::WriteDeltaPacket(GamePacket**p, int stateID) {
	return true;
}

bool NetworkObject::WriteFullPacket(FullPacket& packet) {
    packet.objectID = networkID;
    packet.fullState.position = object.GetTransform().GetPosition();
    packet.fullState.orientation = object.GetTransform().GetOrientation();
    packet.fullState.stateID = lastFullState.stateID++;
    return true;
}

NetworkState& NetworkObject::GetLatestNetworkState() {
	return lastFullState;
}

bool NetworkObject::GetNetworkState(int stateID, NetworkState& state) {
	return false;
}

void NetworkObject::UpdateStateHistory(int minID) {
}