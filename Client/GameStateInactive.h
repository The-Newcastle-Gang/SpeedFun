#pragma once
#include "State.h"
#include <iostream>
#include "./../Client/Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
namespace NCL {
	namespace CSC8503 {
		class Inactive: public State
		{
		public:
			Inactive(GameTechRenderer* rendererRef, GameWorld* gameWorldRef);
			~Inactive();
			void Update(float dt);

			void OnEnter();
			void OnExit();
		protected:
#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;
		};
	}
}
