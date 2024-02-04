#pragma once
#include "State.h"
#include <iostream>
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
#include "GameClient.h"

namespace NCL {
	namespace CSC8503 {
		class MenuState : public State
		{
		public:
			MenuState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef, GameClient* clientRef);
			~MenuState();
			void Update(float dt);

			void OnEnter();
			void OnExit();

            bool IsConnected();
		protected:
#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;
            GameClient* baseClient;


        };
	}
}