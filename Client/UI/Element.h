//
// Created by c3042750 on 10/02/2024.
//

#ifndef CSC8503_ELEMENT_H
#define CSC8503_ELEMENT_H

#include "UIDim.h"
#include "Vector2.h"
#include "Vector4.h"

using namespace NCL;
using namespace Maths;

class Element {
public:
    Element() {}

    Element(UIDim dim) {
        dim = dimensions;
        color = Vector4(1.0, 0.5, 0.2, 1.0);
    }

    [[nodiscard]] UIDim GetDimensions() const {
        return dimensions;
    }

    [[nodiscard]] Vector4 GetColor() const {
        return color;
    }

private:
    UIDim dimensions;
    Vector4 color;
};


#endif //CSC8503_ELEMENT_H
