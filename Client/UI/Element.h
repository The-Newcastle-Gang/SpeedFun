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
#include "TextData.h"

using namespace NCL;

class Element {
public:
    Element(int ind) : OnMouseHover(mouseHover), OnMouseUp(mouseUp), OnMouseDown(mouseDown), OnMouseEnter(mouseEnter), OnMouseExit(mouseExit), OnMouseHold(mouseHold), OnUpdate(update), OnFocusExit(focusExit), OnFocus(focus) {
        dimensions = UIDim();
        color = Vector4(1.0, 1.0, 1.0, 1.0);
        texture = nullptr;
        shader = nullptr;
        hoverTimer = 0;
        mouseDownTimer = 0;
        textData = {};
        index = ind;
        extendUpperX = 0;
        extendLowerX = 0;
        extendLowerY = 0;
        extendUpperY = 0;
        tweenValue1 = 1;
        tweenValue2 = 1;
        isFocused = false;
    }

    [[nodiscard]] UIDim GetDimensions() const {
        return dimensions;
    }

    [[nodiscard]] Vector4 GetColor() const {
        return color;
    }

    [[nodiscard]] Vector2Int& GetAbsoluteSize() {
        return dimensions.absoluteSize;
    }

    [[nodiscard]] Vector2 GetRelativeSize() const {
        return dimensions.relativeSize;
    }

    // TODO: Fix this, should not be returning a reference, tween manager should accept pointer or function callback.
    [[nodiscard]] Vector2Int& GetAbsolutePosition() {
        return dimensions.absolutePosition;
    }

    [[nodiscard]] Vector2 GetRelativePosition() const {
        return dimensions.relativePosition;
    }

    [[nodiscard]] int GetIndex() const {
        return index;
    }

    Element& SetText(const TextData& data) {
        textData = data;
        return *this;
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

    Element& SetShader(ShaderBase* s) {
        shader = s;
        return *this;
    }

    Element& AlignLeft(int padding = 0) {
        dimensions.relativePosition.x = 0.0f;
        dimensions.absolutePosition.x = padding;
        return *this;
    }

    Element& ExtendLowerBounds(int x, int y) {
        extendLowerX = x;
        extendLowerY = y;
        return *this;
    }

    Element& ExtendUpperBounds(int x, int y) {
        extendUpperX = x;
        extendUpperY = y;
        return *this;
    }

    Element& AlignCenter(int padding = 0) {
        dimensions.relativePosition.x = 0.5f - dimensions.relativeSize.x  / 2;
        dimensions.absolutePosition.x = -dimensions.absoluteSize.x / 2 + padding;
        return *this;
    }

    Element& AlignRight(int padding = 0) {
        dimensions.relativePosition.x = 1.0f - dimensions.relativeSize.x;
        dimensions.absolutePosition.x = -dimensions.absoluteSize.x - padding;
        return *this;
    }

    Element& AlignMiddle(int padding = 0) {
        dimensions.relativePosition.y = 0.5f - dimensions.relativeSize.y  / 2;
        dimensions.absolutePosition.y = -dimensions.absoluteSize.y / 2 + padding;
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

    TextData& GetTextData() {
        return textData;
    }

    TextureBase* GetTexture() {
        return texture;
    }

    ShaderBase* GetShader() {
        return shader;
    }

    Element& AddTags(const std::unordered_set<std::string>& pTags) {
        for (auto& t: pTags) {
            tags.insert(t);
        }
        return *this;
    }

    Element& SetId(const std::string& pId) {
        id = pId;
        return *this;
    }

    [[nodiscard]] std::string GetId() const {
        return id;
    }

    Element& SetZIndex(int z) {
        zIndex = z;
        return *this;
    }

    [[nodiscard]] int GetZIndex() const{
        return zIndex;
    }

    void Update(float dt);

    entt::sink<entt::sigh<void(Element&, float)>> OnMouseHover;
    entt::sink<entt::sigh<void(Element&, float)>> OnUpdate;
    entt::sink<entt::sigh<void(Element&)>> OnMouseEnter;
    entt::sink<entt::sigh<void(Element&)>> OnMouseExit;
    entt::sink<entt::sigh<void(Element&)>> OnMouseDown;
    entt::sink<entt::sigh<void(Element&)>> OnMouseUp;
    entt::sink<entt::sigh<void(Element&, float)>> OnMouseHold;
    entt::sink<entt::sigh<void(Element&)>> OnFocus;
    entt::sink<entt::sigh<void(Element&)>> OnFocusExit;

    TextData textData;
    float tweenValue1;
    float tweenValue2;
private:
    UIDim dimensions;
    Vector4 color;
    TextureBase* texture;
    ShaderBase* shader;
    std::string id;
    std::unordered_set<std::string> tags;
    int extendLowerX;
    int extendLowerY;
    int extendUpperX;
    int extendUpperY;
    entt::sigh<void(Element&, float)> mouseHover;
    entt::sigh<void(Element&)> mouseEnter;
    entt::sigh<void(Element&)> mouseExit;
    entt::sigh<void(Element&)> mouseDown;
    entt::sigh<void(Element&)> mouseUp;
    entt::sigh<void(Element&, float)> mouseHold;
    entt::sigh<void(Element&, float)> update;
    entt::sigh<void(Element&)> focus;
    entt::sigh<void(Element&)> focusExit;
    int zIndex;
    bool isFocused;


    int somethingElse = 0;
    int index;
    float hoverTimer;
    float mouseDownTimer;

};


#endif //CSC8503_ELEMENT_H
