//
// Created by c3042750 on 05/02/2024.
//

#include "Resources.h"

MeshGeometry *Resources::GetMesh(const std::string& name, const std::string& type) {
    if (meshes.find(name) == meshes.end()) {
        auto mesh = std::unique_ptr<MeshGeometry>(renderer->LoadMesh(name));
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
