//
// Created by jdhyd on 2/8/2024.
//

#ifndef CSC8503_CLIENTNETWORKDATA_H
#define CSC8503_CLIENTNETWORKDATA_H

#include "SafeQueue.h"
#include "NetworkBase.h"
#include "PacketTypes.h"
#include "NetworkObject.h"
#include "enet.h"

using namespace NCL;
using namespace CSC8503;

class ClientNetworkData {
public:
    SafeQueue<FullPacket> incomingState;
    SafeQueue<FunctionPacket> incomingFunctions;
    SafeQueue<FunctionPacket> outgoingFunctions;
    SafeQueue<InputPacket> outgoingInput;
};


#endif //CSC8503_CLIENTNETWORKDATA_H
