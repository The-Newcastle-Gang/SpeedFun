#pragma once
#include "State.h"
#include <iostream>
#include "./../Client/Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
namespace NCL {
	namespace CSC8503 {
		class Loading : public State
		{
		public:
			Loading(GameTechRenderer* rendererRef, GameWorld* gameWorldRef);
			~Loading();
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
