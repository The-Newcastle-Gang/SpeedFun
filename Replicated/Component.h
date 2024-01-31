#pragma once

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class Component
	{
	public:
		Component() { };
		~Component() {};
		Component(std::shared_ptr<GameObject>  gameObject) { this->gameObject = gameObject;}
		virtual void Update(float dt) {};
		virtual void PhysicsUpdate(float dt) {};
		virtual void Start() {};
		virtual void OnCollisionEnter(std::shared_ptr<GameObject> otherObject) {};
		virtual void OnCollisionStay(std::shared_ptr<GameObject>  otherObject) {};
		virtual void OnCollisionEnd(std::shared_ptr<GameObject>  otherObject) {};

	protected:
		std::shared_ptr<GameObject> gameObject;
	};
}

