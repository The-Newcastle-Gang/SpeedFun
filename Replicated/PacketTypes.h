//
// Created by jdhyd on 2/1/2024.
//

#ifndef CSC8503_PACKETTYPES_H
#define CSC8503_PACKETTYPES_H

#define MAXFUNCTIONDATA 128

#include "Quaternion.h"
#include "Vector2.h"
#include "NetworkBase.h"

using namespace NCL;
using namespace Maths;

struct FunctionData {
    unsigned char data[MAXFUNCTIONDATA];
};

class DataHandler {
public:
    DataHandler(FunctionData* fd) {
        block = fd;
        ptr = block->data;
    }
    // Please avoid calling this with structs.
    template <typename T>
    void Pack(T value) {

    }
    
    template <typename T>
    T Unpack() {

    }
private:
    unsigned char *ptr;
    FunctionData* block;
};

struct InputPacket : public GamePacket {
    Quaternion playerRotation;
    Vector2 playerDirection;
    // Put any more necessary client inputs here

    InputPacket() {
        type = Received_State;
        size = sizeof(InputPacket) - sizeof(GamePacket);
    }
};

struct FunctionPacket : public GamePacket {
    FunctionData data;
    int functionId;
    FunctionPacket(int id, FunctionData *d) {
        type = Function;
        functionId = id;
        memcpy(&data, d, sizeof(FunctionData));
        size = sizeof(FunctionPacket) - sizeof(GamePacket);
    }
};

#endif //CSC8503_PACKETTYPES_H
