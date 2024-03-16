//
// Created by c3042750 on 05/02/2024.
//

#ifndef CSC8503_RESOURCES_H
#define CSC8503_RESOURCES_H

#include "RendererBase.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "Renderer.h"
#include <queue>

using namespace NCL;
using namespace CSC8503;

class Resources {
public:
    Resources(GameTechRenderer* pRenderer) : renderer(pRenderer) {
        shouldThread = false;
        resourceLock.store(false);
    }

    MeshGeometry *GetMesh(const string &name, const std::string &type = "mesh", bool FromThread = false);
    ShaderBase *GetShader(const string &name, bool FromThread = false);
    TextureBase *GetTexture(const string &name, bool FromThread = false);
    MeshAnimation *GetAnimation(const string &name, bool FromThread = false);
    MeshMaterial *GetMeshMaterial(const string &name, bool FromThread = false);
    void ThreadedLoadData(const string &dataType, const string &dataName);

    bool IsLocked() {
        return resourceLock;
    }

    std::atomic<bool>* StartResourceThread();
    void UpdateResourceThread();

private:
    GameTechRenderer* renderer;
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> meshes;
    std::unordered_map<std::string, std::unique_ptr<ShaderBase>> shaders;
    std::unordered_map<std::string, std::unique_ptr<MeshAnimation>> animations;
    std::unordered_map<std::string, std::unique_ptr<MeshMaterial>> meshMaterials;
    std::unordered_map<std::string, std::unique_ptr<TextureBase>> textures;

    struct Job {
        std::string DataType;
        std::string DataName;
    };

    std::queue<Job> jobs;
    std::atomic<bool> shouldThread;
    std::atomic<bool> resourceLock;

    bool AssertLock(bool fromThread);
};


#endif //CSC8503_RESOURCES_H
