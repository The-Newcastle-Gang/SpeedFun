#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include <unordered_map>
#include "Maths.h"

using namespace NCL;
using namespace CSC8503;

class RayEnemyFollow : public Component {
public:
	RayEnemyFollow(GameObject* go);

	void Update(float dt)override;

	void OnPlayerEnter(GameObject* player);

protected:
	GameObject* currentPlayer;
	Vector3 playerPos;
	Vector3 enemyPos;
	Vector3 enemyDir;
	Quaternion enemyOrientation;
	bool isFollow;
};