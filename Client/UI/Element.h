//
// Created by c3042750 on 10/02/2024.
//

#ifndef CSC8503_ELEMENT_H
#define CSC8503_ELEMENT_H

#include "UIDim.h"
#include "Vector2.h"
#include "Vector4.h"
#include "TextureBase.h"

using namespace NCL;
using namespace Maths;
using namespace Rendering;

class Element {
public:
    Element() {
        dimensions = UIDim();
        color = Vector4(1.0, 1.0, 1.0, 1.0);
        texture = nullptr;
    }

    Element(UIDim dim) {
        dimensions = dim;
        color = Vector4(1.0, 1.0, 1.0, 1.0);
        texture = nullptr;
    }

    [[nodiscard]] UIDim GetDimensions() const {
        return dimensions;
    }

    [[nodiscard]] Vector4 GetColor() const {
        return color;
    }

    void SetRelativePosition(Vector2 v) {
        if (v.x >= 0.0f && v.x <= 1.0f && v.y >= 0.0f && v.y <= 1.0f) {
            dimensions.relativePosition = v;
            return;
        }

        std::cerr << "Relative position must be between 0 and 1" << std::endl;
    }

    void SetRelativeSize(Vector2 v) {
        if (v.x >= 0.0f && v.x <= 1.0f && v.y >= 0.0f && v.y <= 1.0f) {
            dimensions.relativeSize = v;
            return;
        }

        std::cerr << "Relative size must be between 0 and 1" << std::endl;
    }

    [[nodiscard]] Vector2Int GetAbsoluteSize() const {
        return dimensions.absoluteSize;
    }

    [[nodiscard]] Vector2 GetRelativeSize() const {
        return dimensions.relativeSize;
    }

    [[nodiscard]] Vector2Int GetAbsolutePosition() const {
        return dimensions.absolutePosition;
    }

    [[nodiscard]] Vector2 GetRelativePosition() const {
        return dimensions.relativePosition;
    }

    void SetAbsolutePosition(Vector2Int v) {
        dimensions.absolutePosition = v;
    }

    void SetAbsoluteSize(Vector2Int v) {
        dimensions.absoluteSize = v;
    }

    Element& AlignLeft(int padding) {
        dimensions.relativePosition.x = 0.0f;
        dimensions.absolutePosition.x = padding;
        return *this;
    }

    Element& AlignCenter() {
    }

    Element& AlignRight(int padding) {
        dimensions.relativePosition.x = 1.0f;
        dimensions.absolutePosition.x = -padding;
        return *this;
    }

    Element& AlignMiddle() {
        return *this;
    }

    Element& AlignBottom(int padding) {
        dimensions.relativePosition.y = 0.0f;
        dimensions.absolutePosition.y = padding;
        return *this;
    }

    Element& AlignTop(int padding) {
        dimensions.relativePosition.y = 1.0f;
        dimensions.absolutePosition.y = -padding;
        return *this;
    }

    Element& SetTexture(TextureBase* tex) {
        texture = tex;
        return *this;
    }

    TextureBase* GetTexture() {
        return texture;
    }

private:
    UIDim dimensions;
    Vector4 color;
    // Set this through canvas, it assigns the textures.
    TextureBase* texture;
};


#endif //CSC8503_ELEMENT_H
