//
// Created by c3042750 on 05/02/2024.
//

#ifndef CSC8503_RESOURCES_H
#define CSC8503_RESOURCES_H

#include "RendererBase.h"
#include "Renderer.h"

class Resources {
public:
    Resources(GameTechRenderer* pRenderer) : renderer(pRenderer) {}

    MeshGeometry *GetMesh(const string &name);
    ShaderBase *GetShader(const string &name);
private:
    GameTechRenderer* renderer;
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> meshes;
    std::unordered_map<std::string, std::unique_ptr<ShaderBase>> shaders;
};


#endif //CSC8503_RESOURCES_H