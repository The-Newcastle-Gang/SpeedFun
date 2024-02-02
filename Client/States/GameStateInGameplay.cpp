#include "GameStateInGameplay.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
using namespace NCL;
using namespace CSC8503;
#
InGameplay::InGameplay(GameTechRenderer* rendererRef, GameWorld* gameWorldRef) : State() {
	renderer = rendererRef;
	world = gameWorldRef;
}

InGameplay::~InGameplay() {

	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
}

void InGameplay::OnEnter() {
	physics = new PhysicsSystem(*world);

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	InitialiseAssets();
}
void InGameplay::OnExit() {
	this->~InGameplay();
	world->ClearAndErase();
	renderer->Render();
}

int InGameplay::ExitType() {

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::L))
	{
		return ExitStates::Win;
	}
	return ExitStates::Invalid;
}

void InGameplay::Update(float dt) {

	Debug::Print("PRESS L TO END GAME", Vector2(10, 20));
	Window::GetWindow()->ShowOSPointer(false);
	Window::GetWindow()->LockMouseToWindow(true);
	world->GetMainCamera()->UpdateCamera(dt);
	if (lockedObject != nullptr) {
		
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}
	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}


void InGameplay::InitialiseAssets() {
	cubeMesh = renderer->LoadMesh("cube.msh");
	sphereMesh = renderer->LoadMesh("sphere.msh");
	charMesh = renderer->LoadMesh("goat.msh");
	enemyMesh = renderer->LoadMesh("Keeper.msh");
	bonusMesh = renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex = renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}


void InGameplay::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}
}

void InGameplay::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

void InGameplay::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	BuildLevelFromJSON("level2");
}

void NCL::CSC8503::InGameplay::BuildLevelFromJSON(std::string levelName)
{
	levelReader = new LevelReader();
	levelBuilder = new LevelBuilder();
	if (!levelReader->ReadLevel(levelName + ".json"))
	{
		cerr << "No file available. Check " + Assets::LEVELDIR << endl;
		return;
	}

	AddCubeToWorld(levelReader->GetStartPosition(), { 1, 1, 1 });
	AddCubeToWorld(levelReader->GetEndPosition(), { 1, 1, 1 });

	for (GroundCubePrimitive* x : levelReader->GetGroundCubes())
	{
		AddCubeToWorld(x->pos, x->dims);
	}

	//levelBuilder->BuildLevel(world);
}

GameObject* NCL::CSC8503::InGameplay::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}
