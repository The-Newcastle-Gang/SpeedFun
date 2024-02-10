//
// Created by c3042750 on 10/02/2024.
//

#ifndef CSC8503_CANVAS_H
#define CSC8503_CANVAS_H

#include "Element.h"

class Canvas {
public:
    Canvas();
    ~Canvas() = default;
    const std::vector<Element>& GetElements();
private:
    std::vector<Element> elements;
};


#endif //CSC8503_CANVAS_H
