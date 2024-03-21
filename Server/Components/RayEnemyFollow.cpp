#include "RayEnemyFollow.h"

using namespace NCL;
using namespace CSC8503;

RayEnemyFollow::RayEnemyFollow(GameObject* go) {
	gameObject = go;
    currentPlayer = nullptr;
}

void RayEnemyFollow::Update(float dt) {

    if (!currentPlayer)return;
    playerPos = currentPlayer->GetTransform().GetPosition();
    enemyPos = gameObject->GetTransform().GetPosition();

    if ((playerPos - enemyPos).Length()<=10.0f) {
        enemyDir = (enemyPos - playerPos).Normalised();
        float transDegree = (atan2(enemyDir.x, enemyDir.z)) * 180.0f / PI;
        enemyOrientation = Quaternion::EulerAnglesToQuaternion(0.0f, transDegree, 0.0f);
        gameObject->GetTransform().SetOrientation(enemyOrientation); //make the enemy will keep facing the player
    }
}


void RayEnemyFollow::OnPlayerEnter(GameObject* player) {
    currentPlayer = player;
}

