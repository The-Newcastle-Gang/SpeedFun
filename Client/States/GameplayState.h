#pragma once
#include "State.h"
#include <iostream>
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
#include "GameClient.h"

namespace NCL {
	namespace CSC8503 {
		enum ExitStates {
			Invalid,
			Win,
			Loss,
			Other
		};
		class GameplayState : public State
		{
		public:
			GameplayState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef, GameClient* clientRef);
			~GameplayState();
			void Update(float dt);

			void OnEnter();
			void OnExit();

            bool IsDisconnected();

			int ExitType();


		protected:

			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();


#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;

            GameClient* baseClient;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			MeshGeometry* capsuleMesh = nullptr;
			MeshGeometry* cubeMesh = nullptr;
			MeshGeometry* sphereMesh = nullptr;

			TextureBase* basicTex = nullptr;
			ShaderBase* basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry* charMesh = nullptr;
			MeshGeometry* enemyMesh = nullptr;
			MeshGeometry* bonusMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;
		};
	}
}