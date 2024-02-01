#include "NetworkBase.h"
#include "enet.h"

NetworkBase::NetworkBase()	{
	netHandle = nullptr;
}

NetworkBase::~NetworkBase()	{
	if (netHandle) {
		enet_host_destroy(netHandle);
	}
}

void NetworkBase::Initialise() {
	if (enet_initialize() != 0) {
        std::cout << "Error occured while intiliasing enet" << std::endl;
    }
}

void NetworkBase::Destroy() {
	enet_deinitialize();
}

bool NetworkBase::ProcessPacket(GamePacket* packet, int peerID) {
	return false;
}