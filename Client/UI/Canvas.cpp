//
// Created by c3042750 on 10/02/2024.
//

#include "Canvas.h"

Canvas::Canvas() {
    layers.insert(std::make_pair("main", CanvasLayer()));
    activeLayers.push_back(&layers["main"]);
}

Canvas::~Canvas() {
    UnloadTextures();
}

void Canvas::UnloadTextures() {
    for (auto& t : textures) {
        delete t.second;
        t.second = nullptr;
    }

    textures.clear();
}

bool Canvas::DoesLayerExist(const std::string& layer) {
    if (layers.find(layer) == layers.end()) {
        return false;
    }

    return true;
}

Element& Canvas::AddElement(const std::string& layerName) {
    if (!DoesLayerExist(layerName)) {
        std::cerr << "Layer doesn't exist!" << std::endl;
    }
    auto index = layers[layerName].GetElements().size();
    layers[layerName].GetElements().emplace_back(index);
    return layers[layerName].GetElements()[index];
}

void Canvas::Reset(bool unloadTextures) {
    if (unloadTextures) {
        UnloadTextures();
    }

    layers.clear();
    // Doesn't matter if canvas is blocking or not.
    layers.insert(std::make_pair("main", CanvasLayer()));
    activeLayers.clear();
    activeLayers.push_back(&layers["main"]);
}

Element& Canvas::AddImageElement(const std::string &name, const std::string& layerName) {
    if (textures.find(name) == textures.end()) {
        textures[name] = TextureLoader::LoadAPITexture(name);
    }

    auto& element = AddElement(layerName);
    element.SetTexture(textures[name]);
    return element;
}

void Canvas::Update(float dt) {
    auto aLayers = GetActiveLayers();
    aLayers.back()->Update(dt);
}

Element& Canvas::GetElementByIndex(int index, const std::string& layerName) {
    if (!DoesLayerExist(layerName)) {
        std::cerr << "Layer doesn't exist!" << std::endl;
    }

    return layers[layerName].GetElements()[index];
}

Element& Canvas::GetElementById(const std::string& id, const std::string& layerName) {
    for (auto& e : layers[layerName].GetElements()) {
        if (e.GetId() == id) {
            return e;
        }
    }

    exit(1);
}

void Canvas::PushActiveLayer(const std::string& layerName) {
    if (!DoesLayerExist(layerName)) {
        std::cerr << "Layer doesn't exist!" << std::endl;
        return;
    }

    if (layerName == "main") {
        std::cerr << "Can't push main!" << std::endl;
        return;
    }

    activeLayers.push_back(&layers[layerName]);
}

void Canvas::PopActiveLayer() {
    if (activeLayers.size() == 1) {
        std::cerr << "Can't pop main!" << std::endl;
        return;
    }

    activeLayers.pop_back();
}

void Canvas::CreateNewLayer(const std::string &name, bool isBlocking) {
    layers.insert(std::make_pair(name, CanvasLayer(isBlocking)));
}

std::vector<CanvasLayer*>& Canvas::GetActiveLayers() {
    return activeLayers;
}

CanvasLayer& Canvas::GetLayer(const std::string& layerName) {
    if (!DoesLayerExist(layerName)) {
        std::cerr << "Layer doesn't exist!" << std::endl;
        return layers["main"];
    }
    return layers[layerName];
}



