#pragma once
#include <memory>

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class Component
	{
		//mixing shared pointers and raw pointers is extremely ugly and awful but the codebase uses raw pointers soooooooo
	public:
		Component() { };
		~Component() {};
		Component(std::shared_ptr<GameObject>  gameObject) { this->gameObject = gameObject;}
		virtual void Update(float dt) {};
		virtual void PhysicsUpdate(float dt) {};
		virtual void Start() {};
		virtual void OnCollisionEnter(const GameObject* otherObject) {};
		virtual void OnCollisionStay(const GameObject* otherObject) {};
		virtual void OnCollisionEnd(const GameObject* otherObject) {};

	protected:
		std::shared_ptr<GameObject> gameObject;
	};
}

