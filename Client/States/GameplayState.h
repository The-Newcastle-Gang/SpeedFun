#pragma once
#include "State.h"
#include <iostream>
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameWorld.h"
namespace NCL {
	namespace CSC8503 {
		enum ExitStates {
			Invalid = 0,
			Win = 1,
			Loss = 2,
			Other = 3
		};
		class GameplayState : public State
		{
		public:
			GameplayState(GameTechRenderer* rendererRef, GameWorld* gameWorldRef);
			~GameplayState();
			void Update(float dt);

			void OnEnter();
			void OnExit();

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