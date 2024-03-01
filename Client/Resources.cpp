//
// Created by c3042750 on 05/02/2024.
//

#include "Resources.h"

MeshGeometry *Resources::GetMesh(const std::string& name, const std::string& type) {
    if (meshes.find(name) == meshes.end()) {
        std::unique_ptr<MeshGeometry> mesh;
        if (name.substr(name.size() - 3) == "obj") {
            mesh = std::unique_ptr<MeshGeometry>(renderer->LoadOBJMesh(name));
        } else {
            mesh = std::unique_ptr<MeshGeometry>(renderer->LoadMesh(name));
        }
        meshes.insert(std::make_pair(name, std::move(mesh)));
        return meshes[name].get();
    }

    return meshes[name].get();
}

ShaderBase *Resources::GetShader(const std::string& name) {
    if (shaders.find(name) == shaders.end()) {
        auto shader = std::unique_ptr<ShaderBase>(renderer->LoadShader(name + std::string(".vert"), name + std::string(".frag")));
        shaders.insert(std::make_pair(name, std::move(shader)));
        return shaders[name].get();
    }

    return shaders[name].get();
}

MeshAnimation *Resources::GetAnimation(const std::string& name) {
    if (animations.find(name) == animations.end()) {
        auto animation = std::unique_ptr<MeshAnimation>(new MeshAnimation(name));
        animations.insert(std::make_pair(name, std::move(animation)));
        return animations[name].get();
    }

    return animations[name].get();
}

MeshMaterial* Resources::GetMeshMaterial(const std::string& name) {
    if (meshMaterials.find(name) == meshMaterials.end()) {
        auto meshMaterial = std::unique_ptr<MeshMaterial>(new MeshMaterial(name));
        meshMaterial->LoadTextures();
        meshMaterials.insert(std::make_pair(name, std::move(meshMaterial)));
        return meshMaterials[name].get();
    }

    return meshMaterials[name].get();
}
