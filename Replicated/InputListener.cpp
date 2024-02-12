//
// Created by idhant on 09-02-2024.
//

#include "InputListener.h"

Vector2 InputListener::PlayerInput;

using namespace NCL;

void InputListener::InputUpdate() {
    //if windows

    if(Window::GetKeyboard()->KeyDown(KeyboardKeys::W)){
        PlayerInput.y = 1;
    }
    else if(Window::GetKeyboard()->KeyDown(KeyboardKeys::S)){
        PlayerInput.y = -1;
    } else{
        PlayerInput.y = 0;
    }

    if(Window::GetKeyboard()->KeyDown(KeyboardKeys::A)){
        PlayerInput.x =-1;
    }
    else if(Window::GetKeyboard()->KeyDown(KeyboardKeys::D)){
        PlayerInput.x = 1;
    } else{
        PlayerInput.x= 0;
    }
    PlayerInput.Normalise();

//DEBUG CODE
    std::string tempv = "PLayer " + std::to_string((int)PlayerInput.x) + " " +  std::to_string((int)PlayerInput.y);
    Debug::Print(tempv, {3, 70}, Debug::GREEN);


}