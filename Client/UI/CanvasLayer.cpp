//
// Created by jdhyd on 2/11/2024.
//

#include "CanvasLayer.h"

CanvasLayer::CanvasLayer(bool shouldBlock) {
    isBlocking = shouldBlock;
}

CanvasLayer::~CanvasLayer() {

}

std::vector<Element>& CanvasLayer::GetElements() {
    return elements;
}

bool CanvasLayer::CheckBlocking() {
    return isBlocking;
}

void CanvasLayer::Update(float dt) {
    for (auto& e : elements) {
        e.Update(dt);
    }
}
