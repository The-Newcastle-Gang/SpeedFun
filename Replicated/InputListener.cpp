//
// Created by idhant on 09-02-2024.
//

#include "InputListener.h"

Vector2 InputListener::PlayerInput;
float InputListener::JumpInput;

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

    if(Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)){
        JumpInput = 1.0f;
    } else {
        JumpInput = 0.0f;
    }

    PlayerInput.Normalise();

//DEBUG CODE
    std::string tempv = "PLayer " + std::to_string(PlayerInput.x) + " " +  std::to_string(PlayerInput.y);
    Debug::Print(tempv, {3, 70}, Debug::GREEN);
    Debug::Print(std::to_string(InputListener::hasPlayerPressed()), {3, 50}, Debug::GREEN);
    Debug::Print("Jumped" + std::to_string(JumpInput), {3,40}, Debug::BLACK);

}

bool InputListener::hasPlayerPressed() {
    return PlayerInput.Length()> 0.5;
}
