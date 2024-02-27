#pragma once

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class Component
	{
	public:
		Component() { };
		virtual ~Component() {};
		Component(GameObject*  gameObject) { this->gameObject = gameObject;}

		//all virtual, not the best approach but works
		virtual void Update(float dt) {};
		//virtual void Update(float dt, GameObject* go,bool isTimeout) {};
		virtual void PhysicsUpdate(float fixedTime) {};
		virtual void Start() {};
		virtual void OnCollisionEnter(const GameObject* otherObject) {};
		virtual void OnCollisionStay(const GameObject* otherObject) {};
		virtual void OnCollisionEnd(const GameObject* otherObject) {};

	protected:
		GameObject* gameObject; //gameObject this component is attached to
	};
}