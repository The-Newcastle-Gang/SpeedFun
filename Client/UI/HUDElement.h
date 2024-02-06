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


class HUDElement {
public:

    //TODO: add these so they can be passed as arguments instead of having to manually set things
    enum preSetPositions {
        CENTER,
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };


    HUDElement();
    ~HUDElement();
    static void CalculateVertexPositions(float xPos, float yPos, float xSize, float ySize);
    static OGLMesh* GetHUDQuad(Vector2 position, float xSize, float ySize);

    static Vector2                     screenPosition;
    static std::vector<Vector3>        vertices;
    static int                         padding;


};


#endif //CSC8503_HUDELEMENT_H
