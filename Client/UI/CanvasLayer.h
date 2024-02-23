//
// Created by jdhyd on 2/11/2024.
//

#ifndef CSC8503_CANVASLAYER_H
#define CSC8503_CANVASLAYER_H

#include <vector>
#include "Element.h"

class CanvasLayer {
public:
    CanvasLayer(bool shouldBlock = true);
    ~CanvasLayer();
    std::vector<Element> &GetElements();
    bool CheckBlocking();
    void Update(float dt);

private:
    bool isBlocking;
    std::vector<Element> elements;
};


#endif //CSC8503_CANVASLAYER_H
