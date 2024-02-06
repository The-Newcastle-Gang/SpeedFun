#pragma once
#include <random>
#include <set>

#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "NetworkObject.h"
namespace NCL {
        class Camera;
        using Maths::Ray;
    namespace CSC8503 {
        class GameObject;
        class Constraint;

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

            bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false, GameObject* ignore = nullptr) const;

            virtual void UpdateWorld(float dt);

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
        };
    }
}

