//
// Created by c3042750 on 05/02/2024.
//

#ifndef CSC8503_RESOURCES_H
#define CSC8503_RESOURCES_H

#include "RendererBase.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "Renderer.h"

class Resources {
public:
    Resources(GameTechRenderer* pRenderer) : renderer(pRenderer) {}

    MeshGeometry *GetMesh(const string &name);
    ShaderBase *GetShader(const string &name);
    MeshAnimation *GetAnimation(const string &name);
    MeshMaterial *GetMeshMaterial(const string &name);

private:
    GameTechRenderer* renderer;
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> meshes;
    std::unordered_map<std::string, std::unique_ptr<ShaderBase>> shaders;
    std::unordered_map<std::string, std::unique_ptr<MeshAnimation>> animations;
    std::unordered_map<std::string, std::unique_ptr<MeshMaterial>> meshMaterials;
};


#endif //CSC8503_RESOURCES_H
