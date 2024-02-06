//
// Created by idhant on 02-02-2024.
//
#include "HUDElement.h"

HUDElement::HUDElement(float x, float y, HudType hd, std::shared_ptr<OGLMesh> PMesh) {
    mesh = std::move(PMesh);
    type = hd;


}

HUDElement::~HUDElement(){

}