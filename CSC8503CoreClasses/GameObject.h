#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "AnimatorObject.h"
#include "Component.h"
#include <entt.hpp>

namespace NCL::CSC8503 {
    class NetworkObject;
    class RenderObject;
    class PhysicsObject;

    enum Tag {
        PLAYER,
        GRAPPLE,
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

        void SetActive(bool pIsActive) {
            isActive = pIsActive;
            onActiveSet.publish(*this, isActive);
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

        entt::sink<entt::sigh<void(GameObject&, bool)>> OnActiveSet;

    protected:

        entt::sigh<void(GameObject&, bool)> onActiveSet;

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
    };
}

