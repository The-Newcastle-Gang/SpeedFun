//
// Created by jdhyd on 2/9/2024.
//

#ifndef CSC8503_SERVERNETWORKDATA_H
#define CSC8503_SERVERNETWORKDATA_H

#include "PacketTypes.h"
#include "NetworkObject.h"
#include "SafeQueue.h"

using namespace NCL;
using namespace CSC8503;

class ServerNetworkData {
public:
    SafeQueue<FullPacket> outgoingState;
    SafeQueue<FunctionPacket> outgoingFunctions;
    SafeQueue<FunctionPacket> incomingFunctions;
    SafeQueue<InputPacket> incomingInput;
};


#endif //CSC8503_SERVERNETWORKDATA_H
