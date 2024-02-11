//
// Created by c3042750 on 10/02/2024.
//

#include "Canvas.h"

Canvas::Canvas() {}

Canvas::~Canvas() {
    for (auto t : textures) {
        delete t.second;
        t.second = nullptr;
    }

    textures.clear();
}

const std::vector<Element>& Canvas::GetElements() {
    return elements;
}

Element& Canvas::AddElement() {
    elements.emplace_back();
    return elements.back().AlignTop(5);
}

void Canvas::Reset() {
    elements.clear();
}

Element& Canvas::AddImageElement(const std::string &name) {
    if (textures.find(name) == textures.end()) {
        textures[name] = TextureLoader::LoadAPITexture(name);
    }

    auto& element = AddElement();
    element.SetTexture(textures[name]);
    return element;
}



