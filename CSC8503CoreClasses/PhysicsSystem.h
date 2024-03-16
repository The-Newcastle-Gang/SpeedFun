#pragma once
#include "GameWorld.h"
#include "PhysicsObject.h"


namespace NCL {
	namespace CSC8503 {

        struct SortSweepStruct {
            float* xPos;
            bool isUpper;
            GameObject* gameObject;

            bool operator<(SortSweepStruct const& other) const {
                return *xPos < *(other.xPos);
            }
        };

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
            void InitialiseSortAndSweepStructs();


			bool GetDebugDrawingCollision() {
				return isDebugDrawingCollision;
			}

            PhysicsMaterial* GetPhysMat(std::string materialName) {
                if (physicsMaterials.find(materialName) != physicsMaterials.end()) {
                    return physicsMaterials[materialName];
                }
                return nullptr;
            }

            void ShouldClearCollisions(bool shouldClear) {
                this->shouldClear = shouldClear;
            }

		protected:
			void BasicCollisionDetection();
			void NarrowPhase();

            void UpdateObjectSortSweepBounds();

            void SetupBounds(GameObject* g);

            void LoadStaticAndDynamicLists();

            void SortAndSweep();

            void SortAndSweepInsertionSort();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();

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
            bool shouldClear = false;
			int numCollisionFrames	    = 5;
            float velocityThreshold    = 8.0f;


            std::vector<GameObject*> staticObjects;
            std::vector<GameObject*> dynamicObjects;
            std::vector<SortSweepStruct> sortAndSweepData;

            std::unordered_map<std::string, PhysicsMaterial*> physicsMaterials;

            std::unordered_map<int, bool> layerMatrix =
              { {DYNAMIC_LAYER | DYNAMIC_LAYER,true},
                  {DYNAMIC_LAYER | PLAYER_LAYER,true},
                  {DYNAMIC_LAYER | STATIC_LAYER,true},
                  {DYNAMIC_LAYER | TRIGGER_LAYER,true},
                  {DYNAMIC_LAYER | OSCILLATOR_LAYER,true},


                  {PLAYER_LAYER | PLAYER_LAYER,true},
                  {PLAYER_LAYER | STATIC_LAYER,true},
                  {PLAYER_LAYER | TRIGGER_LAYER,true},
                  {PLAYER_LAYER | OSCILLATOR_LAYER,true},

                  {STATIC_LAYER | STATIC_LAYER,false},
                  {STATIC_LAYER | TRIGGER_LAYER,false},
                  {STATIC_LAYER | OSCILLATOR_LAYER,false},

                  {TRIGGER_LAYER | TRIGGER_LAYER,false},
                  {TRIGGER_LAYER | OSCILLATOR_LAYER,false},

                  {OSCILLATOR_LAYER | OSCILLATOR_LAYER,false},

              };

		};
	}
}

