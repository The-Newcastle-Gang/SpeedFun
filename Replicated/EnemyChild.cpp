//This is a function that can be placed in a file similar to TutorialGame.
//void TutorialGame::raycastGuard(float dt) {
//	Vector3 rayPos;
//	Vector3 rayDir;
//	Vector3 player1Pos;
//	Vector3 enemyPos;
//	Vector3 enemyDir;
//	Quaternion enemyOrientation;
//
//	player1Pos = playerObject->GetTransform().GetPosition();
//	enemyPos = Enemy1->GetTransform().GetPosition();
//
//	if (enemyPos.x - 50 <= playerObject->GetTransform().GetPosition().x && playerObject->GetTransform().GetPosition().x <= enemyPos.x + 50
//		&& enemyPos.z - 50 <= playerObject->GetTransform().GetPosition().z && playerObject->GetTransform().GetPosition().z <= enemyPos.z + 50) {
//
//		rayDir = (player1Pos - enemyPos).Normalised();
//		rayPos = Enemy1->GetTransform().GetPosition();
//		Debug::DrawLine(rayPos, player1Pos, Vector4(1, 0, 0, 1), 0.1);
//
//		Vector3 enemyDir = (enemyPos - player1Pos).Normalised();
//		float transDegree = Maths::RadiansToDegrees(atan2(enemyDir.x, enemyDir.z));
//		enemyOrientation = Quaternion::EulerAnglesToQuaternion(0.0f, transDegree, 0.0f);
//		Enemy1->GetTransform().SetOrientation(enemyOrientation);
//	}
//}
