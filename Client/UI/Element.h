//
// Created by c3042750 on 10/02/2024.
//

#ifndef CSC8503_ELEMENT_H
#define CSC8503_ELEMENT_H

#include "UIDim.h"
#include "Vector2.h"
#include "Vector4.h"
#include "TextureBase.h"
#include "entt.hpp"
#include "Window.h"

using namespace NCL;
using namespace Maths;
using namespace Rendering;

class Element {
public:
    Element() : OnMouseHover(mouseHover), OnMouseUp(mouseUp), OnMouseDown(mouseDown), OnMouseEnter(mouseEnter), OnMouseExit(mouseExit), OnMouseHold(mouseHold), OnUpdate(update) {
        dimensions = UIDim();
        color = Vector4(1.0, 1.0, 1.0, 1.0);
        texture = nullptr;
        hoverTimer = 0;
        mouseDownTimer = 0;
    }

    [[nodiscard]] UIDim GetDimensions() const {
        return dimensions;
    }

    [[nodiscard]] Vector4 GetColor() const {
        return color;
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

    Element& SetAbsolutePosition(Vector2Int v) {
        dimensions.absolutePosition = v;
        return *this;
    }

    Element& SetAbsoluteSize(Vector2Int v) {
        dimensions.absoluteSize = v;
        return *this;
    }

    Element& SetColor(Vector4 pColor) {
        color = pColor;
        return *this;
    }

    Element& SetRelativePosition(Vector2 v) {
        dimensions.relativePosition = v;
        return *this;
    }

    Element& SetRelativeSize(Vector2 v) {
        dimensions.relativeSize = v;
        return *this;
    }

    Element& AlignLeft(int padding = 0) {
        dimensions.relativePosition.x = 0.0f;
        dimensions.absolutePosition.x = padding;
        return *this;
    }

    Element& AlignCenter() {
        dimensions.relativePosition.x = 0.5f - dimensions.relativeSize.x  / 2;
        dimensions.absolutePosition.x = -dimensions.absoluteSize.x / 2;
        return *this;
    }

    Element& AlignRight(int padding = 0) {
        dimensions.relativePosition.x = 1.0f - dimensions.relativeSize.x;
        dimensions.absolutePosition.x = -dimensions.absoluteSize.x - padding;
        return *this;
    }

    Element& AlignMiddle() {
        dimensions.relativePosition.y = 0.5f - dimensions.relativeSize.y  / 2;
        dimensions.absolutePosition.y = -dimensions.absoluteSize.y / 2;
        return *this;
    }

    Element& AlignBottom(int padding = 0) {
        dimensions.relativePosition.y = 0.0f;
        dimensions.absolutePosition.y = padding;
        return *this;
    }

    Element& AlignTop(int padding = 0) {
        dimensions.relativePosition.y = 1.0f - dimensions.relativeSize.y;
        dimensions.absolutePosition.y = -dimensions.absoluteSize.y - padding;
        return *this;
    }

    Element& SetTexture(TextureBase* tex) {
        texture = tex;
        return *this;
    }

    TextureBase* GetTexture() {
        return texture;
    }

    void Update(float dt);

    entt::sink<entt::sigh<void(Element&, float)>> OnMouseHover;
    entt::sink<entt::sigh<void(Element&, float)>> OnUpdate;
    entt::sink<entt::sigh<void(Element&)>> OnMouseEnter;
    entt::sink<entt::sigh<void(Element&)>> OnMouseExit;
    entt::sink<entt::sigh<void(Element&)>> OnMouseDown;
    entt::sink<entt::sigh<void(Element&)>> OnMouseUp;
    entt::sink<entt::sigh<void(Element&, float)>> OnMouseHold;

private:
    UIDim dimensions;
    Vector4 color;
    TextureBase* texture;
    entt::sigh<void(Element&, float)> mouseHover;
    entt::sigh<void(Element&)> mouseEnter;
    entt::sigh<void(Element&)> mouseExit;
    entt::sigh<void(Element&)> mouseDown;
    entt::sigh<void(Element&)> mouseUp;
    entt::sigh<void(Element&, float)> mouseHold;
    entt::sigh<void(Element&, float)> update;

    int somethingElse = 0;
    float hoverTimer;
    float mouseDownTimer;
};


#endif //CSC8503_ELEMENT_H
