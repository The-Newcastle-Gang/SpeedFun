#include "NetworkObject.h"
#include "enet.h"
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

bool NetworkObject::WritePacket(GamePacket** p, bool deltaFrame, int stateID) {
	if (deltaFrame) {
		if (!WriteDeltaPacket(p, stateID)) {
			return WriteFullPacket(p);
		}
	}
	return WriteFullPacket(p);
}
//Client objects recieve these packets
bool NetworkObject::ReadDeltaPacket(DeltaPacket &p) {
	return true;
}

bool NetworkObject::ReadFullPacket(FullPacket &p) {
    if (p.fullState.stateID < lastFullState.stateID) {
        return false;
    }

    lastFullState = p.fullState;

    object.GetTransform().SetPosition(lastFullState.position);
    object.GetTransform().SetOrientation(lastFullState.orientation);

    return true;
}

bool NetworkObject::WriteDeltaPacket(GamePacket**p, int stateID) {
	return true;
}

bool NetworkObject::WriteFullPacket(GamePacket**p) {
    auto fp = new FullPacket();
    fp->objectID = networkID;
    fp->fullState.position = object.GetTransform().GetPosition();
    fp->fullState.orientation = object.GetTransform().GetOrientation();
    fp->fullState.stateID = lastFullState.stateID++;
    *p = fp;
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