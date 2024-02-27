#include "GameObject.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Debug.h"
using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
    name			= objectName;
    worldID			= -1;
    isActive		= true;
    hasComponent    = false;
    boundingVolume	= nullptr; // Replicated
    networkObject	= nullptr; // Replicated
    physicsObject	= nullptr; // Server
    renderObject	= nullptr; // Client
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;

	for (Component* c : components) {
		delete c;
	}
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
    if (!boundingVolume) {
        return false;
    }
    outSize = broadphaseAABB;
    return true;
}

void GameObject::UpdateBroadphaseAABB() {
    if (!boundingVolume) {
        return;
    }
    if (boundingVolume->type == VolumeType::AABB) {
        broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
    }
    else if (boundingVolume->type == VolumeType::Sphere) {
        float r = ((SphereVolume&)*boundingVolume).GetRadius();
        broadphaseAABB = Vector3(r, r, r);
    }
    else if (boundingVolume->type == VolumeType::OBB) {
        Matrix3 mat = Matrix3(transform.GetOrientation());
        mat = mat.Absolute();
        Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
        broadphaseAABB = mat * halfSizes;
    }
}

void GameObject::DrawCollision()
{
    const CollisionVolume* volume = GetBoundingVolume();
    const VolumeType volumeType = GetBoundingVolume()->type;
    const Transform objTransform = GetTransform();
    Vector4 colour(1, 0, 0, 1);

    switch (volumeType)
    {
    case(VolumeType::AABB):
        Debug::DrawAABBLines(objTransform.GetPosition(), ((AABBVolume&)*volume).GetHalfDimensions(), colour);
        break;

    case(VolumeType::Sphere):
        Debug::DrawSphereLines(objTransform.GetPosition(), objTransform.GetOrientation(), ((SphereVolume&)*volume).GetRadius(), colour);
        break;

    case(VolumeType::Capsule):
        Debug::DrawCapsuleLines(objTransform.GetPosition(),
            objTransform.GetOrientation(),
            ((CapsuleVolume&)*volume).GetRadius() / 2,
            ((CapsuleVolume&)*volume).GetHalfHeight() / 2, colour);
        break;

    case(VolumeType::OBB):
        Debug::DrawOBBLines(objTransform.GetPosition(),
            objTransform.GetOrientation(),
            ((OBBVolume&)*volume).GetHalfDimensions(),
            colour);
        break;
    }
}