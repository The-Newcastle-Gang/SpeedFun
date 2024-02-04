#pragma once
#include "State.h"
#include <iostream>
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
namespace NCL {
	namespace CSC8503 {
		class MenuState : public State
		{
		public:
			MenuState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef);
			~MenuState();
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