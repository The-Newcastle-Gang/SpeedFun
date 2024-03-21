#pragma once
#include "GameObject.h"
#include "Component.h"
#include "Vector3.h"
#include "Vector2.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class TrapBlock : public Component
	{
	public:

		TrapBlock(GameObject* go) { gameObject = go;}

		void Update(float dt)override;

		void OnCollisionEnter(GameObject* otherObject)override;


		void TrapBlockFall(float dt);

	protected:
		double warningTime = 0.0;
		bool blockFall = false;
	};
}



