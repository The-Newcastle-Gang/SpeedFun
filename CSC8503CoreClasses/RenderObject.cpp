#include "RenderObject.h"
#include "MeshGeometry.h"
#include "Transform.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) {
	this->transform	= parentTransform;
    meshScale = parentTransform->GetScale();
    meshOffset = Vector3(0, 0, 0);
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->animatorObject = nullptr;
    this->meshMaterial = nullptr;
}

RenderObject::~RenderObject() {

}