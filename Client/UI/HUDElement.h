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


    HUDElement(Vector2 position, float xSize, float ySize);
    ~HUDElement();
    static void CalculateVertexPositions(float xPos, float yPos, float xSize, float ySize);
    static OGLMesh* GetHUDQuad(Vector2 position, float xSize, float ySize);

    Vector2                     screenPosition;
    static std::vector<Vector3>        vertices;



};


#endif //CSC8503_HUDELEMENT_H
