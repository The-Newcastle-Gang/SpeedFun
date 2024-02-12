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

    template <typename T>
    void Pack(const T& value) {

        if (ptr - block->data + sizeof(T) > MAXFUNCTIONDATA) {
            std::cerr << "Trying to pack too much!" << std::endl;
        }

        memcpy(ptr, &value, sizeof(T));
        ptr += sizeof(T);
    }
    
    template <typename T>
    T Unpack() {

        if (ptr - block->data + sizeof(T) > MAXFUNCTIONDATA) {
            std::cerr << "Trying to unpack too much!" << std::endl;
        }

        T val = *(T*)ptr;
        ptr += sizeof(T);
        return val;
    }
private:
    unsigned char *ptr;
    FunctionData *block;
};

struct InputPacket : public GamePacket {
    Quaternion playerRotation;
    Vector2 playerDirection;
    Vector3 fwdAxis;
    Vector3 rightAxis;
    // Put any more necessary client inputs here

    InputPacket() {
        type = Received_State;
        size = sizeof(InputPacket) - sizeof(GamePacket);
    }
};

struct FunctionPacket : public GamePacket {
    int functionId;
    FunctionData data{};
    FunctionPacket(int id, FunctionData* d) {
        if (d) {
            // Not sure if copy constructor works here, test afterward.
            memcpy(&data, d, sizeof(FunctionData));
        }
        type = Function;
        functionId = id;
        size = sizeof(FunctionPacket) - sizeof(GamePacket);
    }
};

#endif //CSC8503_PACKETTYPES_H
