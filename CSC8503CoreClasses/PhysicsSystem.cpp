#include "PhysicsSystem.h"
#include "PhysicsSystem.h"
#include "PhysicsSystem.h"
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
	applyGravity	= true;
	useBroadPhase	= false;	
	dTOffset		= 0.0f;
	globalDamping	= 0.995f;
	SetGravity(Vector3(0.0f, -13.0f, 0.0f));
    SetupPhysicsMaterials();
}


PhysicsSystem::~PhysicsSystem()	{
    for (auto pair : physicsMaterials) {
        delete pair.second;
        pair.second = nullptr;
    }
}

void PhysicsSystem::SetupPhysicsMaterials() {
    PhysicsMaterial* defaultPhysMat = new PhysicsMaterial();
    physicsMaterials["Default"] = defaultPhysMat;

    PhysicsMaterial* bouncyPhysMat = new PhysicsMaterial();
    bouncyPhysMat->e = 0.995f;
    physicsMaterials["Bouncy"] = bouncyPhysMat;

    auto* playerPhysMat = new PhysicsMaterial();
    playerPhysMat->e = 0.0f;
    physicsMaterials["Player"] = playerPhysMat;

    PhysicsMaterial* triggerPhysMat = new PhysicsMaterial();
    triggerPhysMat->e = 0.0f;
    triggerPhysMat->linearDampingHorizontal = 0.5f;
    physicsMaterials["Trigger"] = triggerPhysMat;
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
        UpdateObjectSortSweepBounds();
	}
  
	int iteratorCount = 0;

	while(dTOffset > realDT) {
        //Update accelerations from external forces
		if (useBroadPhase) {
            SortAndSweep();
			NarrowPhase();
		}
		else {
			BasicCollisionDetection();
		}
        IntegrateAccel(realDT);
		if(isDebugDrawingCollision) DrawAllObjectCollision();

		float constraintDt = realDT /  (float)constraintIterationCount;
		for (int i = 0; i < constraintIterationCount; ++i) {
			UpdateConstraints(constraintDt);	
		}
		IntegrateVelocity(realDT); //update positions from new velocity changes
        gameWorld.UpdateWorldPhysics(realDT);

		dTOffset -= realDT;
        iteratorCount++;
	}

//	ClearForces();	//Once we've finished with the forces, reset them to zero

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

void PhysicsSystem::UpdateCollisionList() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end(); )
    {
        CollisionDetection::CollisionInfo& in = const_cast<CollisionDetection::CollisionInfo&>(*i);

		if ((*i).framesLeft == numCollisionFrames) {
			i->a->OnCollisionBegin(i->b);
			i->b->OnCollisionBegin(i->a);
		}
        CollisionDetection::CollisionInfo blank;
        if(i->a->GetPhysicsObject()->GetIsTriggerVolume()   ||
           i->b->GetPhysicsObject()->GetIsTriggerVolume()   ||
           i->a->GetHasComponent() ||
           i->b->GetHasComponent() )
        {
            in.framesLeft = 4;

            if(!CollisionDetection::ObjectIntersection(i->a,i->b, blank)){
                in.framesLeft = 0;
            }
        }

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
    if (shouldClear) { //we need this in case an OnCollision method needs to clear the collision list
        Clear();
    }
}

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

void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const {

    if(a.GetPhysicsObject()->GetIsTriggerVolume())
    {
        return;
    }
    if(b.GetPhysicsObject()->GetIsTriggerVolume())
    {
        return;
    }

	PhysicsObject* physA = a.GetPhysicsObject();
	PhysicsObject* physB = b.GetPhysicsObject();

	Transform& transformA = a.GetTransform();
	Transform& transformB = b.GetTransform();

	float cRestitution = physA->GetElasticity() * physB->GetElasticity();
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

	if (contactVelocity.Length() < velocityThreshold) {
		cRestitution = 0.0f;
	}

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

void PhysicsSystem::SetupBounds(GameObject* g) {
    SortSweepStruct upperBound;
    upperBound.gameObject = g;
    upperBound.isUpper = true;
    upperBound.xPos = g->GetXUpperBound();

    sortAndSweepData.push_back(upperBound);

    SortSweepStruct lowerBound;
    lowerBound.gameObject = g;
    lowerBound.isUpper = false;
    lowerBound.xPos = g->GetXLowerBound();

    sortAndSweepData.push_back(lowerBound);

}

void PhysicsSystem::LoadStaticAndDynamicLists() {
    std::vector<GameObject*>::const_iterator first;
    std::vector<GameObject*>::const_iterator last;
    gameWorld.GetObjectIterators(first, last);
    for (auto i = first; i != last; ++i) {
        if (!(*i)->GetPhysicsObject()) continue;
        CollisionLayer layer = (*i)->GetPhysicsObject()->GetLayer();
        switch (layer)
        {
        case DYNAMIC_LAYER:
            dynamicObjects.push_back((*i));
            break;
        case PLAYER_LAYER:
            dynamicObjects.push_back((*i));
            break;
        case TRIGGER_LAYER:
            staticObjects.push_back((*i));
            break;
        case STATIC_LAYER:
            staticObjects.push_back((*i));
            break;
        case OSCILLATOR_LAYER:
            dynamicObjects.push_back((*i));
            break;
        case MAX_LAYER:
            break;
        default:
            break;
        }
    }
}

void PhysicsSystem::InitialiseSortAndSweepStructs() {
    LoadStaticAndDynamicLists();
    for (GameObject* gameObject : staticObjects) {
        gameObject->UpdateBroadphaseXBounds();
    }
    UpdateObjectSortSweepBounds();

    for (GameObject* gameObject : staticObjects) {
        SetupBounds(gameObject);
    }
    for (GameObject* gameObject : dynamicObjects) {
        SetupBounds(gameObject);
    }
}

void PhysicsSystem::UpdateObjectSortSweepBounds() {
    for (GameObject* gameObject : dynamicObjects) { //dont need to update statics!
        gameObject->UpdateBroadphaseXBounds();
    }
}

void PhysicsSystem::SortAndSweep() {
    broadphaseCollisions.clear();

    SortAndSweepInsertionSort();
    std::set<GameObject*> currentValidObjects;
    for (int i = 0; i < sortAndSweepData.size(); i++) {
        SortSweepStruct currentBound = sortAndSweepData[i];
        GameObject* currentBoundObject = currentBound.gameObject;
        if (!currentBound.isUpper) {
            for (GameObject* other : currentValidObjects) {
                if (!layerMatrix[other->GetPhysicsObject()->GetLayer() | currentBoundObject->GetPhysicsObject()->GetLayer()])continue;

                CollisionDetection::CollisionInfo info;
                info.a = Tmin(currentBoundObject, other);
                info.b = Tmax(currentBoundObject, other);
                broadphaseCollisions.insert(info);
            }
            currentValidObjects.insert(currentBoundObject);
        }
        else {
            currentValidObjects.erase(currentBoundObject);
        }
    }

}

void PhysicsSystem::SortAndSweepInsertionSort() { //adapted from https://www.geeksforgeeks.org/cpp-program-for-insertion-sort/
    SortSweepStruct currentValue;
    for (int i = 1; i < sortAndSweepData.size(); i++) {
        currentValue = sortAndSweepData[i];
        int j = i - 1;

        while (j >= 0 && currentValue < sortAndSweepData[j]) {
            sortAndSweepData[j + 1] = sortAndSweepData[j];
            j = j - 1;
        }
        sortAndSweepData[j + 1] = currentValue;
    }
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

		float inverseMass = object->GetInverseMass();       //getting inverse mass
        if (inverseMass == 0.0f)continue;

		Vector3 linearVel = object->GetLinearVelocity();
		Vector3 force = object->GetForce();

        object->ClearForces();
		Vector3 accel = force * inverseMass;                     //f*m^-1

        if (applyGravity && inverseMass > 0) {
            accel += gravity;
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
    float frameLinearDamping = 1.0f - (0.1f * dt);

	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		PhysicsObject* object = (*i)->GetPhysicsObject();

		if (object == nullptr) {
			continue;
		}
        if (object->GetInverseMass() == 0.0f)continue;

		Transform& transform = (*i)->GetTransform();

		Vector3 position = transform.GetPosition();
		Vector3 linearVel = object->GetLinearVelocity();

		position += linearVel * dt;
		transform.SetPosition(position);

        linearVel = linearVel * frameLinearDamping;
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