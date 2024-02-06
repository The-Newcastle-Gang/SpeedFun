//
// Created by idhant on 02-02-2024.
//
#include "HUDElement.h"

HUDElement::HUDElement(Vector2 position, float xSize, float ySize) {
    screenPosition = position;
    CalculateVertexPositions(position.x, position.y , xSize, ySize);
}

void HUDElement::CalculateVertexPositions(float xPos, float yPos, float xSize, float ySize){

    auto absX =(float)((xPos/100)- 0.5);
    auto absY =(float)((yPos/100)- 0.5);
    vertices = {Vector3(absX,            absY ,             0.0f),
                Vector3(absX,            absY + ySize/100,  0.0f),
                Vector3(absX + xSize/100,absY + ySize/100,  0.0f),
                Vector3(absX + xSize/100,absY,              0.0f)};
}

OGLMesh* HUDElement::GetHUDQuad(){
    auto temp = new OGLMesh();
    temp->SetVertexPositions({vertices[0],vertices[1],vertices[2],vertices[3]});
    temp->SetVertexIndices({0,1,2,2,3,0});
    temp->UploadToGPU();
    return temp;
    delete temp;
}

HUDElement::~HUDElement(){

}