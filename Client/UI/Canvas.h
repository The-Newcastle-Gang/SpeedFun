//
// Created by c3042750 on 10/02/2024.
//

#ifndef CSC8503_CANVAS_H
#define CSC8503_CANVAS_H

#include "Element.h"
#include "TextureLoader.h"
#include "TextureBase.h"

using namespace NCL;
using namespace Rendering;

class Canvas {
public:
    Canvas();
    ~Canvas();
    const std::vector<Element>& GetElements();
    Element& AddElement();
    Element& AddImageElement(const std::string &name);
    // Clear all elements on canvas
    void Reset();
private:
    std::vector<Element> elements;
    // Because of my silly brain canvas can't include resources because circular dependencies, so have to
    // put in their own texture loader.
    std::unordered_map<std::string, TextureBase*> textures;
};


#endif //CSC8503_CANVAS_H
