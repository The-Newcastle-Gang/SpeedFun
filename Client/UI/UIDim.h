//
// Created by c3042750 on 10/02/2024.
//

#ifndef CSC8503_UDIM2_H
#define CSC8503_UDIM2_H

#include "Vector2.h"

using namespace NCL;
using namespace Maths;

struct Vector2Int {
    Vector2Int() {
        x = 0;
        y = 0;
    };
    Vector2Int(int x, int y) {
        this->x = x;
        this->y = y;
    }
    Vector2Int(Vector2 v) {
        x = (int)v.x;
        y = (int)v.y;
    }
    int x;
    int y;
};

class UIDim {
public:
    friend class Element;

    UIDim() = default;
    ~UIDim() = default;

    UIDim(Vector2 relS, Vector2Int absS, Vector2 relP, Vector2Int absP) : absolutePosition(absP), absoluteSize(absS), relativePosition(relP), relativeSize(relS) {}

private:
    Vector2Int absolutePosition;
    Vector2Int absoluteSize;
    Vector2 relativePosition;
    Vector2 relativeSize;
};


#endif //CSC8503_UDIM2_H
