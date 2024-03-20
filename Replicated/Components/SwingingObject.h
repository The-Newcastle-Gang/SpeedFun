#pragma once
#include "GameObject.h"
#include "Component.h"

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;

	const float MAX_TIMER = 6.5f;
	const float MIN_TIMER = 3.0f;

	class SwingingObject : public Component
	{
	public:

		SwingingObject(GameObject* go) { gameObject = go; Start(); }
		void Update(float dt)override;

		void Start()override {
			startingPos = gameObject->GetTransform().GetPosition();
			timer = MAX_TIMER;
			CheckTimerState();
		}

        void OnCollisionEnter(GameObject* otherObject) override;
        void OnCollisionStay(GameObject* otherObject) override;
        void OnCollisionEnd(GameObject* otherObject) override;

		void PhysicsUpdate(float dt)override {}

		void CheckTimerState();

		void SetRadius(float r) { radius = r; }

	protected:
		Vector3 startingPos;
		float radius = 10;
		float timer;
		float ascending;
	};
}

