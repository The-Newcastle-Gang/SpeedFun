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



//    HUDElement(float x, float y);
    HUDElement(Vector2 position, float xSize, float ySize);
    ~HUDElement();
    void CalculateVertexPositions(float xPos, float yPos, float xSize, float ySize);
    OGLMesh* GetHUDQuad();
    std::vector<Vector3> GetVertices() const { return vertices;}


    Vector2                     screenPosition;
    std::shared_ptr<OGLMesh>    mesh;
    HUDElement::HudType         type;
    std::vector<Vector3>        vertices;
    GLuint                      tex;


};


#endif //CSC8503_HUDELEMENT_H
