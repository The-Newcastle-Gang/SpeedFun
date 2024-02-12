//
// Created by idhant on 09-02-2024.
//

#include "InputListener.h"

Vector2 InputListener::HorizontalAxis;
Vector2 InputListener::VerticalAxis;

using namespace NCL;

void InputListener::InputUpdate() {
    //ifdef windows

    if(Window::GetKeyboard()->KeyDown(KeyboardKeys::W)){
        VerticalAxis = {0,1};
    }
    else if(Window::GetKeyboard()->KeyDown(KeyboardKeys::S)){
        VerticalAxis = {0,-1};
    } else{
        VerticalAxis = {0,0};
    }

    if(Window::GetKeyboard()->KeyDown(KeyboardKeys::A)){
        HorizontalAxis = {1,0};
    }
    else if(Window::GetKeyboard()->KeyDown(KeyboardKeys::D)){
        HorizontalAxis = {-1,0};
    } else{
        HorizontalAxis = {0,0};
    }


//DEBUG CODE
    std::string tempv = "Vertical " + std::to_string((int)VerticalAxis.x) + " " +  std::to_string((int)VerticalAxis.y);
    std::string temph = "Horizontal " + std::to_string((int)HorizontalAxis.x)+ " " +  std::to_string((int)HorizontalAxis.y);
    Debug::Print(tempv, {3, 70}, Debug::GREEN);
    Debug::Print(temph, {3, 60}, Debug::RED);

}