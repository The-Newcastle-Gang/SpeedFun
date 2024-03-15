#pragma once
#include "GameObject.h"
#include "Component.h"
#include "Vector3.h"
#include "Vector2.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"

#include <functional>

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class TestSpeedUpBlock : public Component
	{
	public:

		TestSpeedUpBlock(GameObject* go) { gameObject = go; }

		void Update(float dt)override;

		void OnCollisionEnter(GameObject* otherObject)override;

	protected:
		bool isSpeedUp;
		double timer;
	};
}