#pragma once
#include "State.h"
#include <iostream>
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
namespace NCL {
	namespace CSC8503 {
		class Menu : public State
		{
		public:
			Menu(GameTechRenderer* rendererRef, GameWorld* gameWorldRef);
			~Menu();
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