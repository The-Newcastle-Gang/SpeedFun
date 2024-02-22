//
// Created by c3042750 on 10/02/2024.
//

#ifndef CSC8503_CANVAS_H
#define CSC8503_CANVAS_H

#include "Element.h"
#include "TextureLoader.h"
#include "TextureBase.h"
#include "CanvasLayer.h"

using namespace NCL;
using namespace Rendering;

class Canvas {
public:
    Canvas();
    ~Canvas();

    Element& AddElement(const std::string& layerName = "main");
    Element& AddImageElement(const std::string& name, const std::string& layerName = "main");
    std::vector<CanvasLayer*>& GetActiveLayers();
    CanvasLayer& GetLayer(const std::string& layerName);
    // Clear all elements on canvas
    void Reset(bool unloadTextures = false);
    void Update(float dt);
    void CreateNewLayer(const std::string& name, bool isBlocking = true);
    void PushActiveLayer(const std::string &layerName);
    void PopActiveLayer();
    std::string GetActiveLayer();


    Element& GetElementByIndex(int index, const std::string &layerName = "main");

    bool DoesLayerExist(const std::string &layer);

    Element &GetElementById(const string &id, const string &layerName = "main");

private:
    std::unordered_map<std::string, CanvasLayer> layers;
    std::vector<CanvasLayer*> activeLayers;
    // Because of my silly brain canvas can't include resources because circular dependencies, so have to
    // put in their own texture loader.
    std::unordered_map<std::string, TextureBase*> textures;
    void UnloadTextures();

};


#endif //CSC8503_CANVAS_H
