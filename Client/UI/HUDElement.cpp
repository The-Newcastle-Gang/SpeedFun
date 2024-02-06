//
// Created by idhant on 02-02-2024.
//
#include "HUDElement.h"

std::vector<Vector3> HUDElement::vertices;


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

OGLMesh* HUDElement::GetHUDQuad(Vector2 position, float xSize, float ySize){
    auto temp = new OGLMesh();
    CalculateVertexPositions(position.x,position.y,xSize, ySize);

    if(vertices.empty()){
        std::cerr << "UI Build failed, check HUDElement.";
    }

    temp->SetVertexPositions({vertices[0],vertices[1],vertices[2],vertices[3]});
    temp->SetVertexIndices({0,1,2,2,3,0});
    temp->UploadToGPU();
    return temp;

}

HUDElement::~HUDElement(){

}

