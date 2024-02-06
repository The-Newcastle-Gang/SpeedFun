#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "Component.h"
using std::vector;
using NCL::CSC8503::Component;

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

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetName() const {
			return name;
		}

		bool GetBroadphaseAABB(Vector3&outsize) const;

		void UpdateBroadphaseAABB();

		void SetWorldID(int newID) {
			worldID = newID;
		}

		int		GetWorldID() const {
			return worldID;
		}

		//returns true if component found, false if not
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

	protected:
		std::vector<Component*> components;

		Transform			transform;

		CollisionVolume*	boundingVolume;
		PhysicsObject*		physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;

		bool		isActive;
		int			worldID;
		std::string	name;

		Vector3 broadphaseAABB;
	};
}

