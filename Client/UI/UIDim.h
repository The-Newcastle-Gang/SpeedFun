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

    UIDim() = default;
    ~UIDim() = default;

    UIDim(Vector2 relS, Vector2Int absS, Vector2 relP, Vector2Int absP) {
        SetAbsoluteSize(absS);
        SetRelativeSize(relS);
        SetAbsolutePosition(absP);
        SetRelativePosition(relP);
    }

    [[nodiscard]] Vector2Int GetAbsoluteSize() const {
        return absoluteSize;
    }

    [[nodiscard]] Vector2 GetRelativeSize() const {
        return relativeSize;
    }

    [[nodiscard]] Vector2Int GetAbsolutePosition() const {
        return absolutePosition;
    }

    [[nodiscard]] Vector2 GetRelativePosition() const {
        return relativePosition;
    }

    void SetRelativePosition(Vector2 v) {
        if (v.x >= 0.0f && v.x <= 1.0f && v.y >= 0.0f && v.y <= 1.0f) {
            relativePosition = v;
            return;
        }

        std::cerr << "Relative position must be between 0 and 1" << std::endl;
    }

    void SetRelativeSize(Vector2 v) {
        if (v.x >= 0.0f && v.x <= 1.0f && v.y >= 0.0f && v.y <= 1.0f) {
            relativeSize = v;
            return;
        }

        std::cerr << "Relative size must be between 0 and 1" << std::endl;
    }

    void SetAbsolutePosition(Vector2Int v) {
        absolutePosition = v;
    }

    void SetAbsoluteSize(Vector2Int v) {
        absoluteSize = v;
    }

private:
    Vector2Int absolutePosition;
    Vector2Int absoluteSize;
    Vector2 relativePosition;
    Vector2 relativeSize;
};


#endif //CSC8503_UDIM2_H
