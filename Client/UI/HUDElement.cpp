//
// Created by idhant on 02-02-2024.
//
#include "HUDElement.h"

std::vector<Vector3> HUDElement::vertices;
Vector2 HUDElement::screenPosition;
int HUDElement::padding =10;


HUDElement::HUDElement() {

}

HUDElement::~HUDElement(){

}

void HUDElement::CalculateVertexPositions(float xPos, float yPos, float xSize, float ySize){

    auto absX =(float)((xPos-50.0f)/ 50.0f);
    auto absY =(float)((yPos-50.0f)/ 50.0f);

    vertices = {Vector3(absX,            absY ,             0.0f),
                Vector3(absX,            absY + ySize/100,  0.0f),
                Vector3(absX + xSize/100,absY + ySize/100,  0.0f),
                Vector3(absX + xSize/100,absY,              0.0f)};
}

OGLMesh* HUDElement::GetHUDQuad(Vector2 position, float xSize, float ySize){
    screenPosition = position;

    auto temp = new OGLMesh();
    CalculateVertexPositions(position.x,position.y,xSize, ySize);

    if(vertices.empty()){
        std::cerr << "UI Build failed, check HUDElement.cpp";
    }

    temp->SetVertexPositions({vertices[0],vertices[1],vertices[2],vertices[3]});
    temp->SetVertexIndices({0,1,2,2,3,0});
    temp->UploadToGPU();
    return temp;

}


