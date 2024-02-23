#pragma once
#include "GameWorld.h"
#include "PhysicsObject.h"


namespace NCL {
	namespace CSC8503 {
		class PhysicsSystem	{
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void Clear();

			void Update(float dt);

			void UseGravity(bool state) {
				applyGravity = state;
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);

			void SetDebugDrawingCollision(bool b) {
				isDebugDrawingCollision = b;
			}

			bool GetDebugDrawingCollision() {
				return isDebugDrawingCollision;
			}

            PhysicsMaterial* GetPhysMat(std::string materialName) {
                if (physicsMaterials.find(materialName) != physicsMaterials.end()) {
                    return physicsMaterials[materialName];
                }
                return nullptr;
            }

		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

            void SetupPhysicsMaterials();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) const;
			void DrawAllObjectCollision();


			bool isDebugDrawingCollision = false;

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;

			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set<CollisionDetection::CollisionInfo> broadphaseCollisions;
			std::vector<CollisionDetection::CollisionInfo> broadphaseCollisionsVec;

			bool useBroadPhase		    = true;
			int numCollisionFrames	    = 5;
      float velocityThreshold    = 8.0f;


      std::unordered_map<std::string, PhysicsMaterial*> physicsMaterials;

		};
	}
}

