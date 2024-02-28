#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
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

        RenderObject* GetRenderObject() const {
            return renderObject;
        }

        PhysicsObject* GetPhysicsObject() const {
            return physicsObject;
        }

        NetworkObject* GetNetworkObject() const {
            return networkObject;
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

        const std::string& GetName() const {
            return name;
        }

        bool GetBroadphaseAABB(Vector3& outsize) const;

        void UpdateBroadphaseAABB();

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

    protected:

        CollisionVolume*	boundingVolume;
        PhysicsObject*		physicsObject;
        RenderObject*		renderObject;
        NetworkObject*		networkObject;

        bool		isActive;
        bool        hasComponent;
        int			worldID;
        std::string	name;

        Tag tag;

        Vector3 broadphaseAABB;
    };
}

