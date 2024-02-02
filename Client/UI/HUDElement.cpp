//
// Created by idhant on 02-02-2024.
//
#include "HUDElement.h"

HUDElement::HUDElement(float x, float y, HudType hd) {
    //do some calcs to convert into %
    screenPosition.x = x;
    screenPosition.y = y;
    type = hd;
}

HUDElement::~HUDElement(){

}