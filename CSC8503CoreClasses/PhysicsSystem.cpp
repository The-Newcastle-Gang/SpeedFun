#include "PhysicsSystem.h"
#include "GameObject.h"
#include "CollisionDetection.h"
#include "Quaternion.h"

#include "Constraint.h"

#include "Debug.h"
#include <functional>
#include <GameTimer.h>

using namespace NCL;
using namespace CSC8503;

PhysicsSystem::PhysicsSystem(GameWorld& g) : gameWorld(g)	{
	applyGravity	= false;
	useBroadPhase	= false;	
	dTOffset		= 0.0f;
	globalDamping	= 0.995f;
	SetGravity(Vector3(0.0f, -9.8f, 0.0f));
    SetupPhysicsMaterials();
}


PhysicsSystem::~PhysicsSystem()	{
    for (auto pair : physicsMaterials) {
        delete pair.second;
    }
}

void PhysicsSystem::SetupPhysicsMaterials() {
    PhysicsMaterial* defaultPhysMat = new PhysicsMaterial();
    physicsMaterials["Default"] = defaultPhysMat;

    PhysicsMaterial* bouncyPhysMat = new PhysicsMaterial();
    bouncyPhysMat->e = 0.995f;
    physicsMaterials["Bouncy"] = bouncyPhysMat;
}

void PhysicsSystem::SetGravity(const Vector3& g) {
	gravity = g;
}

/*

If the 'game' is ever reset, the PhysicsSystem must be
'cleared' to remove any old collisions that might still
be hanging around in the collision list. If your engine
is expanded to allow objects to be removed from the world,
you'll need to iterate through this collisions list to remove
any collisions they are in.

*/
void PhysicsSystem::Clear() {
	allCollisions.clear();
}

/*

This is the core of the physics engine update

*/

bool useSimpleContainer = false;

int constraintIterationCount = 10;

//This is the fixed timestep we'd LIKE to have
const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;

/*
This is the fixed update we actually have...
If physics takes too long it starts to kill the framerate, it'll drop the 
iteration count down until the FPS stabilises, even if that ends up
being at a low rate. 
*/
int realHZ		= idealHZ;
float realDT	= idealDT;

void PhysicsSystem::Update(float dt) {

	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!

	GameTimer t;
	t.GetTimeDeltaSeconds();

	if (useBroadPhase) {
		UpdateObjectAABBs();
	}
	int iteratorCount = 0;
	while(dTOffset > realDT) {
		gameWorld.UpdateWorldPhysics(realDT);
		IntegrateAccel(realDT); //Update accelerations from external forces
		if (useBroadPhase) {
			BroadPhase();
			NarrowPhase();
		}
		else {
			BasicCollisionDetection();
		}
		if(isDebugDrawingCollision) DrawAllObjectCollision();
		//This is our simple iterative solver - 
		//we just run things multiple times, slowly moving things forward
		//and then rechecking that the constraints have been met		
		float constraintDt = realDT /  (float)constraintIterationCount;
		for (int i = 0; i < constraintIterationCount; ++i) {
			UpdateConstraints(constraintDt);	
		}
		IntegrateVelocity(realDT); //update positions from new velocity changes

		dTOffset -= realDT;
		iteratorCount++;
	}

	ClearForces();	//Once we've finished with the forces, reset them to zero

	UpdateCollisionList(); //Remove any old collisions

	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();

	//Uh oh, physics is taking too long...
	if (updateTime > realDT) {
		realHZ /= 2;
		realDT *= 2;
		std::cout << "Dropping iteration count due to long physics time...(now " << realHZ << ")\n";
	}
	else if(dt*2 < realDT) { //we have plenty of room to increase iteration count!
		int temp = realHZ;
		realHZ *= 2;
		realDT /= 2;

		if (realHZ > idealHZ) {
			realHZ = idealHZ;
			realDT = idealDT;
		}
		if (temp != realHZ) {
			std::cout << "Raising iteration count due to short physics time...(now " << realHZ << ")\n";
		}
	}
}

/*
Later on we're going to need to keep track of collisions
across multiple frames, so we store them in a set.

The first time they are added, we tell the objects they are colliding.
The frame they are to be removed, we tell them they're no longer colliding.

From this simple mechanism, we we build up gameplay interactions inside the
OnCollisionBegin / OnCollisionEnd functions (removing health when hit by a 
rocket launcher, gaining a point when the player hits the gold coin, and so on).
*/
void PhysicsSystem::UpdateCollisionList() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end(); ) {
		if ((*i).framesLeft == numCollisionFrames) {
			i->a->OnCollisionBegin(i->b);
			i->b->OnCollisionBegin(i->a);
		}

		CollisionDetection::CollisionInfo& in = const_cast<CollisionDetection::CollisionInfo&>(*i);
		in.framesLeft--;

		if ((*i).framesLeft < 0) {
			i->a->OnCollisionEnd(i->b);
			i->b->OnCollisionEnd(i->a);
			i = allCollisions.erase(i);
		}
		else {
			++i;
		}
	}
}

void PhysicsSystem::UpdateObjectAABBs() {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		(*i)->UpdateBroadphaseAABB();
	}
	/*gameWorld.OperateOnContents(
		[](GameObject* g) {
			g->UpdateBroadphaseAABB();
		}
	);*/
}

/*

This is how we'll be doing collision detection in tutorial 4.
We step thorugh every pair of objects once (the inner for loop offset 
ensures this), and determine whether they collide, and if so, add them
to the collision set for later processing. The set will guarantee that
a particular pair will only be added once, so objects colliding for
multiple frames won't flood the set with duplicates.
*/
void PhysicsSystem::BasicCollisionDetection() {
    std::vector<GameObject*>::const_iterator first;
    std::vector<GameObject*>::const_iterator last;
    gameWorld.GetObjectIterators(first, last);

    for(auto i = first; i!=last; ++i){
        if((*i)->GetPhysicsObject() == nullptr){
            continue;
        }

        //we start this i+1 since we never have to check for objects colliding with themselves
        for(auto j = i+1; j!= last; j++){
            if((* j)->GetPhysicsObject() == nullptr){
                continue;
            }
            CollisionDetection::CollisionInfo info;

            if(CollisionDetection::ObjectIntersection(*i,*j,info)){
                //std::cout<< "Collision between " << (*i)->GetName() << " and " << (*j)->GetName() << " at "<< info.point.penetration<< "\n";
                ImpulseResolveCollision(*info.a, *info.b, info.point);
                info.framesLeft = numCollisionFrames;
                allCollisions.insert(info);
            }
        }
    }
}

/*

In tutorial 5, we start determining the correct response to a collision,
so that objects separate back out. 

*/
void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const {

	PhysicsObject* physA = a.GetPhysicsObject();
	PhysicsObject* physB = b.GetPhysicsObject();

	Transform& transformA = a.GetTransform();
	Transform& transformB = b.GetTransform();

	float cRestitution = physA->GetElasticity() + physB->GetElasticity() * 0.5;

	//there is probably a better way of doing this
	/*if (physA->isPlayerRet() || physB->isPlayerRet()) {
		cRestitution = 0.0f;
	}*/

	float totalMass = physA->GetInverseMass() + physB->GetInverseMass();

	if (totalMass == 0) {
		return;
	}
	//projection method here lol
	transformA.SetPosition(transformA.GetPosition() - p.normal * p.penetration * (physA->GetInverseMass() / totalMass));
	transformB.SetPosition(transformB.GetPosition() + p.normal * p.penetration * (physB->GetInverseMass() / totalMass));

	Vector3 relativeA = p.localA;
	Vector3 relativeB = p.localB;

	Vector3 angVelocityA = Vector3::Cross(physA->GetAngularVelocity(), relativeA);
	Vector3 angVelocityB = Vector3::Cross(physB->GetAngularVelocity(), relativeB);

	Vector3 fullVelocityA = physA->GetLinearVelocity() + angVelocityA;
	Vector3 fullVelocityB = physB->GetLinearVelocity() + angVelocityB;

	Vector3 contactVelocity = fullVelocityB - fullVelocityA;

	//who cares?  i surely don't
    //re: this unironically came to bite me back in my behind, funny.
//	if (contactVelocity.Length() < 8.0f) {
//		cRestitution = 0.0f;
//	}

	float impulseForce = Vector3::Dot(contactVelocity, p.normal);

	Vector3 inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, p.normal), relativeA);
	Vector3 inertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, p.normal), relativeB);

	float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);

	float j = (-(1.0f + cRestitution) * impulseForce) / (totalMass + angularEffect);
	Vector3 fullImpulse = p.normal * j;

	physA->ApplyLinearImpulse(-fullImpulse);
	physB->ApplyLinearImpulse(fullImpulse);

	physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -fullImpulse));
	physB->ApplyAngularImpulse(Vector3::Cross(relativeB, fullImpulse));

}

template <class T>
T Tmin(T a, T b) {
	return a < b ? a : b;
}

template <class T>
T Tmax(T a, T b) {
	return a > b ? a : b;
}

/*

Later, we replace the BasicCollisionDetection method with a broadphase
and a narrowphase collision detection method. In the broad phase, we
split the world up using an acceleration structure, so that we can only
compare the collisions that we absolutely need to. 

*/
void PhysicsSystem::BroadPhase() {
	broadphaseCollisions.clear();
	QuadTree<GameObject*> tree(Vector2(1024, 1024), 7, 6);

	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	gameWorld.GetObjectIterators(first, last);

	//Creates the tree lawl
	for (auto i = first; i != last; ++i) {
		Vector3 halfSizes;
		if (!(*i)->GetBroadphaseAABB(halfSizes)) {
			continue;
		}
		Vector3 pos = (*i)->GetTransform().GetPosition();
		tree.Insert(*i, pos, halfSizes);
	}

	//now we actually have to see what sets of objects may collide

	//this is a lambda function thats sending said thigns into the listr and capturing to it
	tree.OperateOnContents(
		[&](std::list<QuadTreeEntry<GameObject*>>& data) {
			CollisionDetection::CollisionInfo info;
			for (auto i = data.begin(); i != data.end(); ++i) {
				for (auto j = std::next(i); j != data.end(); ++j) {
					info.a = Tmin((*i).object, (*j).object);
					info.b = Tmax((*i).object, (*j).object);
					broadphaseCollisions.insert(info);
				}
			}
		});
}


/*

The broadphase will now only give us likely collisions, so we can now go through them,
and work out if they are truly colliding, and if so, add them into the main collision list
*/
void PhysicsSystem::NarrowPhase() {
	//iterating through every broad phase pair and then checking if it actually interacts
	for (std::set<CollisionDetection::CollisionInfo>::iterator
		i = broadphaseCollisions.begin(); i != broadphaseCollisions.end(); ++i) {

		CollisionDetection::CollisionInfo info = *i;

		//if said collision happens, resolve it
		if (CollisionDetection::ObjectIntersection(info.a, info.b, info)) {
			info.framesLeft = numCollisionFrames;
			ImpulseResolveCollision(*info.a, *info.b, info.point);

			allCollisions.insert(info);
		}

	}
}

/*
Integration of acceleration and velocity is split up, so that we can
move objects multiple times during the course of a PhysicsUpdate,
without worrying about repeated forces accumulating etc. 

This function will update both linear and angular acceleration,
based on any forces that have been accumulated in the objects during
the course of the previous game frame.
*/
void PhysicsSystem::IntegrateAccel(float dt) {

	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	gameWorld.GetObjectIterators(first, last);          //setting our constructors to the first and last game objects in the scene

	for (auto i = first; i != last; i++) {

		PhysicsObject* object = (*i)->GetPhysicsObject();     //Get the physics object from the game obj

		if (object == nullptr) {                                //if it's not physical then we go to the next one
			continue;
		}
		//if (object->GetType() == PhysObjType::Static || object->GetType() == PhysObjType::Sleeping) { continue; }


		float inverseMass = object->GetInverseMass();       //getting inverse mass

		Vector3 linearVel = object->GetLinearVelocity();
		Vector3 force = object->GetForce();
		Vector3 accel = force * inverseMass;                     //f*m^-1

		if (applyGravity && inverseMass > 0) {
			//if ((*i)->GetGType() != GameObjType::AI) {
				accel += gravity;
			//}
		}


		linearVel += accel * dt;                                // v = a(t).dt
		object->SetLinearVelocity(linearVel);

		Vector3 torque = object->GetTorque();
		Vector3 angVel = object->GetAngularVelocity();

		object->UpdateInertiaTensor();                          //Basically this is getting the moment of intertia

		Vector3 angAccel = object->GetInertiaTensor() * torque;

		angVel += angAccel * dt;                                //IW = tau . dt
		object->SetAngularVelocity(angVel);

	}
}

/*
This function integrates linear and angular velocity into
position and orientation. It may be called multiple times
throughout a physics update, to slowly move the objects through
the world, looking for collisions.
*/
void PhysicsSystem::IntegrateVelocity(float dt) {

	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		PhysicsObject* object = (*i)->GetPhysicsObject();

		if (object == nullptr) {
			continue;
		}

		Transform& transform = (*i)->GetTransform();

		Vector3 position = transform.GetPosition();
		Vector3 linearVel = object->GetLinearVelocity();

		position += linearVel * dt;
		transform.SetPosition(position);

        linearVel.x = linearVel.x * (1.0f - (object->GetLinearDampHorizontal() * dt));
        linearVel.z = linearVel.z * (1.0f - (object->GetLinearDampHorizontal() * dt));

        linearVel.y = linearVel.y * (1.0f - (object->GetLinearDampVertical() * dt));

		object->SetLinearVelocity(linearVel);

		Quaternion orientation = transform.GetOrientation();
		Vector3 angVel = object->GetAngularVelocity();

		orientation = orientation +
			(Quaternion(angVel * dt * 0.5f, 0.0f) * orientation);
		orientation.Normalise();

		transform.SetOrientation((orientation));

		float frameAngDamp = 1.0f - (object->GetAngularDamp() * dt);
		angVel = angVel * frameAngDamp;
		object->SetAngularVelocity(angVel);
	}
}
void PhysicsSystem::DrawAllObjectCollision() {
	gameWorld.OperateOnContents(
		[](GameObject* o) {
			o->DrawCollision();
		}
	);
}


/*
Once we're finished with a physics update, we have to
clear out any accumulated forces, ready to receive new
ones in the next 'game' frame.
*/
void PhysicsSystem::ClearForces() {
	gameWorld.OperateOnContents(
		[](GameObject* o) {
			o->GetPhysicsObject()->ClearForces();
		}
	);
}


/*

As part of the final physics tutorials, we add in the ability
to constrain objects based on some extra calculation, allowing
us to model springs and ropes etc. 

*/
void PhysicsSystem::UpdateConstraints(float dt) {
	std::vector<Constraint*>::const_iterator first;
	std::vector<Constraint*>::const_iterator last;
	gameWorld.GetConstraintIterators(first, last);

	for (auto i = first; i != last; ++i) {
		(*i)->UpdateConstraint(dt);
	}
}