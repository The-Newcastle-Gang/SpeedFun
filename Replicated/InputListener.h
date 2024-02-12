//
// Created by idhant on 09-02-2024.
//
#ifndef CSC8503_INPUTLISTENER_H
#define CSC8503_INPUTLISTENER_H

#include <Vector2.h>
#include "Window.h"
#include "Debug.h"

using namespace NCL;
using namespace Maths;

class InputListener {
public:
    InputListener();
    ~InputListener();

    static void InputUpdate();
protected:
    static Vector2 HorizontalAxis;
    static Vector2 VerticalAxis;

};


#endif //CSC8503_INPUTLISTENER_H
