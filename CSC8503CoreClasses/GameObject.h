#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "AnimatorObject.h"
#include "Component.h"

namespace NCL::CSC8503 {
    class NetworkObject;
    class RenderObject;
    class PhysicsObject;

	class GameObject{
	public:
		GameObject(std::string name = "");
		~GameObject();

		virtual void Update(float dt) { UpdateAllComponents(dt);}
		virtual void PhysicsUpdate(float dt) { PhysicsUpdateAllComponents(dt); }
		virtual void Start() { StartAllComponents(); }

		virtual void OnCollisionBegin(const GameObject* otherObject) {
			for (Component* component : components)component->OnCollisionEnter(otherObject);
		}

		virtual void OnCollisionEnd(const GameObject* otherObject) {
			for (Component* component : components)component->OnCollisionEnd(otherObject);
		}

		virtual void OnCollisionStay(const GameObject* otherObject) {
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

        bool GetBroadphaseAABB(Vector3&outsize) const;

        void UpdateBroadphaseAABB();

        void SetWorldID(int newID) {
            worldID = newID;
        }
    
        virtual void OnCollisionBegin(GameObject* otherObject) {
            //std::cout << "OnCollisionBegin event occured!\n";
        }

        virtual void OnCollisionEnd(GameObject* otherObject) {
            //std::cout << "OnCollisionEnd event occured!\n";
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
		}

		std::vector<Component*> components;

		Transform transform;
        
        int GetWorldID() const {
          return worldID;
        }

        void DrawCollision();

        [[nodiscard]] bool GetIsPlayerBool() const { return isPlayer; }
        void SetIsPlayerBool(bool b) { isPlayer = b;  }

    protected:

        CollisionVolume*	boundingVolume;
        PhysicsObject*		physicsObject;
        RenderObject*		renderObject;
        NetworkObject*		networkObject;
        AnimatorObject*		    animatorObject;

        bool		isActive;
        bool        isPlayer;
        int			worldID;
        std::string	name;

        Vector3 broadphaseAABB;
    };
}

