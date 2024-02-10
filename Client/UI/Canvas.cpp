//
// Created by c3042750 on 10/02/2024.
//

#include "Canvas.h"

Canvas::Canvas() {
    elements.emplace_back(UIDim(Vector2(0.0f, 0.0f), Vector2Int(50, 50), Vector2(0.0f, 0.0f), Vector2Int(10,10)));
}


const std::vector<Element>& Canvas::GetElements() {
    return elements;
}

