#pragma once
#include <random>
#include <set>

#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "NetworkObject.h"
#include "PhysicsObject.h"

namespace NCL {
        class Camera;
        using Maths::Ray;
    namespace CSC8503 {
        class GameObject;
        class Constraint;

        struct PointLightInfo {
            Vector4		lightColour;
            float		lightRadius;
            Vector3		lightPosition;
        };

        typedef std::function<void(GameObject*)> GameObjectFunc;
        typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

        class GameWorld	{
        public:
            GameWorld();
            ~GameWorld();

            void Clear();
            void ClearAndErase();

            void AddGameObject(GameObject* o, bool isNetworked);
            void RemoveGameObject(GameObject* o, bool andDelete = false);

            void AddConstraint(Constraint* c);
            void RemoveConstraint(Constraint* c, bool andDelete = false);

            Camera* GetMainCamera() const {
                return mainCamera;
            }

            void ShuffleConstraints(bool state) {
                shuffleConstraints = state;
            }

            void ShuffleObjects(bool state) {
                shuffleObjects = state;
            }

            bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false, GameObject* ignore = nullptr, int layerMask = ~MAX_LAYER) const;

			virtual void UpdateWorld(float dt);
			virtual void StartWorld();
			virtual void UpdateWorldPhysics(float dt);

            void OperateOnContents(GameObjectFunc f);

            void GetObjectIterators(
                GameObjectIterator& first,
                GameObjectIterator& last) const;

            void GetConstraintIterators(
                std::vector<Constraint*>::const_iterator& first,
                std::vector<Constraint*>::const_iterator& last) const;

            int GetWorldStateID() const {
                return worldStateCounter;
            }

            GameObject* GetObjectByNetworkId(int networkId) {
                return networkObjects[networkId]->GetParent();
            }

            [[nodiscard]] NetworkObject* GetNetworkObject(int networkId) const {
                return networkObjects[networkId];
            }

            std::vector<NetworkObject*>::const_iterator GetNetworkIteratorStart() {
                return networkObjects.begin();
            }

            std::vector<NetworkObject*>::const_iterator GetNetworkIteratorEnd() {
                return networkObjects.end();
            }

            void AddPointLightToWorld(PointLightInfo pointLight) {
                pointLights.push_back(pointLight);
            }

            std::vector<PointLightInfo>* GetPointLights() { return &pointLights; }

        protected:
            std::vector<GameObject*> gameObjects;
            std::vector<Constraint*> constraints;

            std::vector<NetworkObject*> networkObjects;

            Camera* mainCamera;

            bool shuffleConstraints;
            bool shuffleObjects;
            int		worldIDCounter;
            int		worldStateCounter;

            int networkIdCounter;

            std::vector<PointLightInfo> pointLights;

        };
    }
}

