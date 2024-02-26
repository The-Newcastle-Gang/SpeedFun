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
		virtual void PhysicsUpdate(float fixedTime) {};
		virtual void Start() {};
		virtual void OnCollisionEnter(GameObject* otherObject) {};
		virtual void OnCollisionStay(GameObject* otherObject) {};
		virtual void OnCollisionEnd(GameObject* otherObject) {};

	protected:
		GameObject* gameObject; //gameObject this component is attached to
	};
}