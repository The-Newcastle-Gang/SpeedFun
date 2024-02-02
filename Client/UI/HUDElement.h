//
// Created by idhant on 02-02-2024.
//

#ifndef CSC8503_HUDELEMENT_H
#define CSC8503_HUDELEMENT_H

#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include <vector>
#include "Vector2.h"

using namespace NCL;
using namespace Rendering;
using namespace Maths;

//hud element class, will contain mesh, texture, shader, and maybe type.
//will be passed into renderer to be rendered lol

class HUDElement {
public:
    enum HudType {
        STATIC_RECT,
        DYNAMIC_RECT
    };

    struct HudScreenPosition{
        float x;
        float y;
    };


    HUDElement(float x, float y, HudType ht);
    ~HUDElement();

    HudScreenPosition           screenPosition;
    std::shared_ptr<OGLShader>  hudShader;
    std::shared_ptr<OGLMesh>    hudMesh;
    HUDElement::HudType         type;
    std::vector<Vector2>        vertices;

};


#endif //CSC8503_HUDELEMENT_H
