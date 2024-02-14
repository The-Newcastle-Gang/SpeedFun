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
    static Vector2 GetPlayerInput(){ return PlayerInput; }
    static float GetJumpInput(){return JumpInput;}
    static bool    hasPlayerPressed();
protected:
    static Vector2 PlayerInput;
    static float JumpInput;
    static float GrappleInput;

};


#endif //CSC8503_INPUTLISTENER_H
