#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "CapsuleVolume.h"
#include "SphereVolume.h"
#include "AnimatorObject.h"
#include "Component.h"

namespace NCL::CSC8503 {
    class NetworkObject;
    class RenderObject;
    class PhysicsObject;

    enum Tag {
        PLAYER
    };

    class GameObject {
    public:
        GameObject(std::string name = "");
        ~GameObject();

        virtual void Update(float dt) { UpdateAllComponents(dt); }
        virtual void PhysicsUpdate(float dt) { PhysicsUpdateAllComponents(dt); }
        virtual void Start() { StartAllComponents(); }

        virtual void OnCollisionBegin(GameObject* otherObject) {
            for (Component* component : components)component->OnCollisionEnter(otherObject);
        }

        virtual void OnCollisionEnd(GameObject* otherObject) {
            for (Component* component : components)component->OnCollisionEnd(otherObject);
        }

        virtual void OnCollisionStay(GameObject* otherObject) {
            for (Component* component : components)component->OnCollisionStay(otherObject);
        }

        void UpdateAllComponents(float dt) { for (Component* component : components)component->Update(dt); }

        void PhysicsUpdateAllComponents(float fixedTime) { for (Component* component : components)component->PhysicsUpdate(fixedTime); }

        void StartAllComponents() { for (Component* component : components)component->Start(); }


        void SetBoundingVolume(CollisionVolume* vol) {
            boundingVolume = vol;
            float halfDim = 0.0f;
            switch (vol->type)
            {
                case VolumeType::AABB: {
                    halfDim = ((AABBVolume*)vol)->GetHalfDimensions().x;
                    break;
                }
                case VolumeType::OBB: {
                    Vector3 halfDims = ((OBBVolume*)vol)->GetHalfDimensions();
                    halfDim = std::max(halfDims.x, halfDims.y);
                    halfDim = std::max(halfDim, halfDims.z);
                    break;
                }
                case VolumeType::Capsule: {
                    halfDim = ((CapsuleVolume*)vol)->GetHalfHeight();
                    break;
                }
                case VolumeType::Sphere: {
                    halfDim = ((SphereVolume*)vol)->GetRadius();
                    break;
                }
                default:
                    break;
            }
            SetBroadXHalfDim(halfDim);
        }

        const CollisionVolume* GetBoundingVolume() const {
            return boundingVolume;
        }

        bool IsActive() const {
            return isActive;
        }

        Transform& GetTransform() {
            return transform;
        }

        Transform* GetTransformPointer() {
            return &transform;
        }

        RenderObject* GetRenderObject() const {
            return renderObject;
        }

        PhysicsObject* GetPhysicsObject() const {
            return physicsObject;
        }

        NetworkObject* GetNetworkObject() const {
            return networkObject;
        }

        AnimatorObject* GetAnimatorObject() const {
            return animatorObject;
        }

        void SetRenderObject(RenderObject* newObject) {
            renderObject = newObject;
        }

        void SetPhysicsObject(PhysicsObject* newObject) {
            physicsObject = newObject;
        }

        void SetNetworkObject(NetworkObject* newObject) {
            networkObject = newObject;
        }

        void SetAnimatorObject(AnimatorObject* newObject) {
            animatorObject = newObject;
        }

        const std::string& GetName() const {
            return name;
        }

        bool GetBroadphaseAABB(Vector3& outsize) const;

        void UpdateBroadphaseAABB();

        void SetBroadXHalfDim(float halfDim);

        void UpdateBroadphaseXBounds();

        void SetWorldID(int newID) {
            worldID = newID;
        }

        template <typename T>
        bool TryGetComponent(T*& returnPointer) {
            for (Component* component : components) {
                T* typeCast = dynamic_cast<T*>(component);
                if (typeCast) {
                    returnPointer = typeCast;
                    return true;
                }
            }
            return false;
        }

        void AddComponent(Component* component) {
            components.push_back(component);
            SetHasComponent(true);
        }

        std::vector<Component*> components;

        Transform transform;

        int GetWorldID() const { return worldID; }

        const Tag GetTag() { return tag; }
        void SetTag(Tag t) { tag = t; }

        const bool GetHasComponent() { return hasComponent; }
        void SetHasComponent(bool b) { hasComponent = b; }

        void DrawCollision();

        [[nodiscard]] Vector3 GetCurrentCheckPointPos() const { return checkPointPos; }
        void SetCurrentCheckPointPos(Vector3 v) { checkPointPos = v;  }


        float* GetXLowerBound() { return &broadXLowerBound; }
        float* GetXUpperBound() { return &broadXUpperBound; }

    protected:

        CollisionVolume*	boundingVolume;
        PhysicsObject*		physicsObject;
        RenderObject*		renderObject;
        NetworkObject*		networkObject;
        AnimatorObject*		    animatorObject;

        Vector3 checkPointPos;

        bool		isActive;
        bool        hasComponent;
        int			worldID;
        std::string	name;

        Tag tag;

        Vector3 broadphaseAABB;

        float broadXLowerBound = 0.0f;
        float broadXUpperBound = 0.0f;

        float broadXHalfDim = 0.0f;

    };
}

