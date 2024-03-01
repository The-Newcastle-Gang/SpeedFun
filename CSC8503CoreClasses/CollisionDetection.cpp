#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "Window.h"
#include "Maths.h"
#include "Debug.h"

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;

		case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) {
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;


	Vector3 raypos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i) {                                   
		if (rayDir[i] > 0) {
			tVals[i] = (boxMin[i] - raypos[i]) / rayDir[i];
		}
		else if (rayDir[i] < 0) {
			tVals[i] = (boxMax[i] - raypos[i]) / rayDir[i];
		}
	}

	float bestT = tVals.GetMaxElement();                      
	if (bestT < 0.0f) {                                           
		return false;
	}

	Vector3 intersection = raypos + (rayDir * bestT);          
	const float epsilon = 0.0001f;                             

	for (int i = 0; i < 3; ++i) {                                   
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i]) {
			return false;
		}
	}

	collision.collidedAt = intersection;
	collision.rayDistance = bestT;

	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) {

	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();

	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) {

	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());                                          

	Vector3 localRayPos = r.GetPosition() - position;                                                    

	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());                

	bool collided = RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision);

	if (collided) {
		collision.collidedAt = transform * collision.collidedAt + position;

	}

	return collided;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) {

	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();


	Vector3 dir = (spherePos - r.GetPosition());                        //vec pointing from us to sphere
	float sphereProj = Vector3::Dot(dir, r.GetDirection());       // projecting the point on the proj

	if (sphereProj < 0.0f) {
		return false;
	}

	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);  /*getting the closest point on the sphere*/
	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius) {                                      //outside the sphere
		return false;
	}

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - (offset);
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);

	return true;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) {

	Vector3 capCenter = worldTransform.GetPosition();
	Quaternion capRota = worldTransform.GetOrientation().Normalised();

	Vector3 capsuleDir = Matrix3(capRota) * Vector3(0, 1, 0);
	Vector3 capTop = capCenter + capsuleDir * volume.GetHalfHeight();
	Vector3 capBottom = capCenter - capsuleDir * volume.GetHalfHeight();


	Vector3 dire = (capCenter - r.GetPosition());
	float proj = Vector3::Dot(r.GetDirection(), dire);

	//getting closest point on the vector along the ray direction
	Vector3 c = r.GetPosition() + (r.GetDirection() * proj);

	//getting closest point on the capsule line segment
	Vector3 de = Maths::ClosestPointLineSegment(capTop, capBottom, c);
	Transform temp;
	temp.SetPosition(de);

	return RaySphereIntersection(r, temp, SphereVolume(volume.GetRadius()), collision);
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	//Two AABBs
	if (pairType == VolumeType::AABB) {
        auto result = AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
		return result;
	}
	//Two Spheres
	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	//Two OBBs
	if (pairType == VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}
	//Two Capsules
	if (pairType == VolumeType::Capsule) {
		return CapsuleIntersection((CapsuleVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}

	//AABB vs Sphere pairs
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		//we are only changing this becaue the parameters for func below expects aabb first
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//OBB vs sphere pairs
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere) {
		return OBBSphereIntersection((OBBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBSphereIntersection((OBBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//Capsule vs other interactions
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		return SphereCapsuleIntersection((CapsuleVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((CapsuleVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		return AABBCapsuleIntersection((CapsuleVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}
	if (volB->type == VolumeType::Capsule && volA->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBCapsuleIntersection((CapsuleVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::OBB) {
		return OBBCapsuleIntersection((CapsuleVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}
	if (volB->type == VolumeType::Capsule && volA->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBCapsuleIntersection((CapsuleVolume&)*volB, transformB, (OBBVolume&)*volA, transformA, collisionInfo);

	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::AABB) {
		return OBBAABBIntersection((OBBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}
	if (volB->type == VolumeType::AABB && volA->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBAABBIntersection((OBBVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);

	}

	return false;
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;

	if (abs(delta.x) < totalSize.x &&
		abs(delta.y) < totalSize.y &&
		abs(delta.z) < totalSize.z) {
		return true;
	}
	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	//get position of everything, the boxes and then send them into the test function to see if overlap happens

	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);

	if (overlap) {
		static const Vector3 faces[6] = {
		  Vector3(-1,0,0), Vector3(1,0,0),
		  Vector3(0,-1,0), Vector3(0,1,0),
		  Vector3(0,0,-1), Vector3(0,0,1),
		};

		//getting the max and the min position of each the faces by adding and subbing size from pos
		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;

		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;

		//amount of overlap on each axis -  how much of B's maximum position goes into A
		float distances[6] = {
				(maxB.x - minA.x),
				(maxA.x - minB.x),
				(maxB.y - minA.y),
				(maxA.y - minB.y),
				(maxB.z - minA.z),
				(maxA.z - minB.z)
		};

		float penetration = FLT_MAX;
		Vector3 bestAxis;

		//go through each of the distances form face and then find the smallest value since it will always collide first

		for (int i = 0; i < 6; i++) {
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}

		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration);
		return true;
	}
	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penentration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penentration);
		return true;
	}
	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	//getting the dist from center to edge
	Vector3 boxSize = volumeA.GetHalfDimensions();
	//center to center vector
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	//getting the closest point between the two objects
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

	//now we get how far along this vector the closest point is
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	//if this closest point is smaller than the radius then it collides
	if (distance < volumeB.GetRadius()) {
		Vector3 collisionNormal = localPoint.Normalised();
		//remove the distance from the radius because its like overlapping
		float penentration = (volumeB.GetRadius() - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penentration);
		return true;
	}
	return false;
}

bool  CollisionDetection::OBBSphereIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Quaternion boxRota = worldTransformA.GetOrientation();

	Matrix3 boxTransform = Matrix3(boxRota);
	Matrix3 invTransform = Matrix3(boxRota.Conjugate());

	Vector3 localSpaceSphPos = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	Transform tempTransform;
	tempTransform.SetPosition((invTransform * localSpaceSphPos) + worldTransformA.GetPosition());

	bool collided = AABBSphereIntersection(AABBVolume(volumeA.GetHalfDimensions()), worldTransformA, volumeB, tempTransform, collisionInfo);

	if (collided) {
		collisionInfo.point.localA = boxTransform * collisionInfo.point.localA;
		collisionInfo.point.localB = boxTransform * collisionInfo.point.localB;
		collisionInfo.point.normal = localSpaceSphPos.Normalised();
	}
	return collided;
}

bool CollisionDetection::OBBAABBIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionDetection::CollisionInfo& collisionInfo) {
	OBBVolume temp = OBBVolume(volumeB.GetHalfDimensions());
	Transform tempTransform;

	tempTransform.SetPosition(worldTransformB.GetPosition());
	tempTransform.SetOrientation(Quaternion(0.0f, 0.0f, 0.0f, 1.0f));
	tempTransform.SetScale(worldTransformB.GetScale());

	bool collided = OBBIntersection(volumeA, worldTransformA, temp, tempTransform, collisionInfo);

	if (collided) {
		collisionInfo.point.normal = -collisionInfo.point.normal;
	}
	return collided;
}


bool CollisionDetection::AABBCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 capCenter = worldTransformA.GetPosition();
	Vector3 boxCenter = worldTransformB.GetPosition();
	Quaternion capRota = worldTransformA.GetOrientation().Normalised();

	Vector3 capsuleDir = Matrix3(capRota) * Vector3(0, 1, 0);
	Vector3 capTop = capCenter + capsuleDir * volumeA.GetHalfHeight();
	Vector3 capBottom = capCenter - capsuleDir * volumeA.GetHalfHeight();

	Vector3 delta = worldTransformA.GetPosition() - worldTransformB.GetPosition();
	Vector3 boxSize = volumeB.GetHalfDimensions();
	Vector3 closestBoxPoint = worldTransformB.GetPosition() + Maths::Clamp(delta, -boxSize, boxSize);

	Vector3 pointToCapsuleDir = closestBoxPoint - worldTransformA.GetPosition();
	float proj = Vector3::Dot(capsuleDir, pointToCapsuleDir);
	Vector3 capsulePoint = worldTransformA.GetPosition() + (capsuleDir * proj);


	capsulePoint = Maths::ClosestPointLineSegment(capTop, capBottom, capsulePoint);

	float pointDistance = (capsulePoint - closestBoxPoint).Length();
	float offset = volumeA.GetRadius();

	if (pointDistance < offset) {
		Vector3 colNormal = pointToCapsuleDir.Normalised();
		float pen = (volumeA.GetRadius() - pointDistance);

		Vector3 lA = colNormal * volumeA.GetRadius();
		Vector3 lB = Vector3();

		collisionInfo.AddContactPoint(lA, lB, colNormal, pen);
		return true;
	}

	return false;
}


bool CollisionDetection::OBBCapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB,
	CollisionInfo& collisionInfo) {
	Quaternion boxRota = worldTransformB.GetOrientation();

	Matrix3 boxTransform = Matrix3(boxRota);
	Matrix3 invTransform = Matrix3(boxRota.Conjugate());

	Vector3 localSpaceCapPos = worldTransformA.GetPosition() - worldTransformB.GetPosition();

	Transform tempTransform;
	tempTransform.SetPosition((invTransform * localSpaceCapPos) + worldTransformB.GetPosition());
	tempTransform.SetOrientation((invTransform * worldTransformA.GetOrientation()));

	bool collided = AABBCapsuleIntersection(volumeA, tempTransform, AABBVolume(volumeB.GetHalfDimensions()), worldTransformB, collisionInfo);

	if (collided) {
		
		collisionInfo.point.localA = boxTransform * collisionInfo.point.localA;
		collisionInfo.point.localB = boxTransform * collisionInfo.point.localB;
		collisionInfo.point.normal = boxTransform * collisionInfo.point.normal;

	}
	return collided;
}

bool CollisionDetection::SphereCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 sphCen = worldTransformB.GetPosition();
	Vector3 capCenter = worldTransformA.GetPosition();
	Quaternion capRot = worldTransformA.GetOrientation().Normalised();

	Vector3 capsuleDir = Matrix3(capRot) * Vector3(0, 1, 0);
	Vector3 capTop = capCenter + capsuleDir * volumeA.GetHalfHeight();
	Vector3 capBottom = capCenter - capsuleDir * volumeA.GetHalfHeight();;

	float dist = Maths::SqDistPointSegment(capTop, capBottom, sphCen);

	float radiusSum = volumeB.GetRadius() + volumeA.GetRadius();

	if (dist <= radiusSum * radiusSum) {

		Vector3 d = Maths::ClosestPointLineSegment(capTop, capBottom, sphCen);
		Transform temp;
		temp.SetPosition(d);

		return(SphereIntersection(SphereVolume(volumeA.GetRadius()), temp, volumeB, worldTransformB, collisionInfo));
	}

	return false;
}

bool CollisionDetection::CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const CapsuleVolume& volumeB, const Transform& worldTransformB,
	CollisionInfo& collisionInfo) {

	Vector3 center1 = worldTransformA.GetPosition();
	Quaternion rota1 = worldTransformA.GetOrientation().Normalised();
	Vector3 capsuleDir = Matrix3(rota1) * Vector3(0, 1, 0);
	Vector3 c1Top = center1 + capsuleDir * volumeA.GetHalfHeight();
	Vector3 c1Bot = center1 - capsuleDir * volumeA.GetHalfHeight();

	Vector3 center2 = worldTransformB.GetPosition();
	Quaternion rota2 = worldTransformB.GetOrientation().Normalised();
	Vector3 capsuleDir2 = Matrix3(rota2) * Vector3(0, 1, 0);
	Vector3 c2Top = center2 + capsuleDir2 * volumeB.GetHalfHeight();
	Vector3 c2Bot = center2 - capsuleDir2 * volumeB.GetHalfHeight();

	Vector3 closest1 = Maths::ClosestPointLineSegment(c1Top, c1Bot, center2);
	Vector3 closest2 = Maths::ClosestPointLineSegment(c2Top, c2Bot, closest1);

	Transform temp;
	temp.SetPosition(closest1);

	Transform temp2;
	temp2.SetPosition(closest2);

	bool collided = SphereIntersection(SphereVolume(volumeA.GetRadius()), temp,
		(SphereVolume(volumeB.GetRadius())), temp2,
		collisionInfo);

	return collided;
}

Vector3* CollisionDetection::CalculateEdgeNormals(const Transform& inTransform,
	const Transform& inBTransform) {

	Vector3* edgeNormals = new Vector3[15];

	Quaternion inRota = inTransform.GetOrientation();
	Quaternion inRotaB = inBTransform.GetOrientation();

	edgeNormals[0] = inRota * Vector3(1, 0, 0); //Xa
	edgeNormals[1] = inRota * Vector3(0, 1, 0); //Ya
	edgeNormals[2] = inRota * Vector3(0, 0, 1); //za

	edgeNormals[3] = inRotaB * Vector3(1, 0, 0); //Xb
	edgeNormals[4] = inRotaB * Vector3(0, 1, 0); //Yb
	edgeNormals[5] = inRotaB * Vector3(0, 0, 1); //Zb

	int i = 6;

	for (int j = 0; j < 3; ++j) {
		for (int k = 3; k < 6; ++k) {
			edgeNormals[i] = Vector3::Cross(edgeNormals[j], edgeNormals[k]).Normalised(); //Normalixed world axis
			i++;
		}
	}
	return edgeNormals;

}

Vector3* CollisionDetection::CalculateVerticesOBB(const OBBVolume& inVolume, const Transform& inTransform) {
	Vector3* listVertices = new Vector3[8];

	Vector3 halfDims = inVolume.GetHalfDimensions();

	listVertices[0] = Vector3(-halfDims.x, -halfDims.y, -halfDims.z);
	listVertices[1] = Vector3(halfDims.x, -halfDims.y, -halfDims.z);
	listVertices[2] = Vector3(halfDims.x, halfDims.y, -halfDims.z);
	listVertices[3] = Vector3(-halfDims.x, halfDims.y, -halfDims.z);
	listVertices[4] = Vector3(-halfDims.x, -halfDims.y, halfDims.z);
	listVertices[5] = Vector3(halfDims.x, -halfDims.y, halfDims.z);
	listVertices[6] = Vector3(halfDims.x, halfDims.y, halfDims.z);
	listVertices[7] = Vector3(-halfDims.x, halfDims.y, halfDims.z);

	// Rotate the local vertices using the orientation
	for (int i = 0; i < 8; ++i) {
		listVertices[i] = inTransform.GetOrientation() * listVertices[i];
	}

	// Translate the rotated vertices to the world position
	for (int i = 0; i < 8; ++i) {
		listVertices[i] += inTransform.GetPosition();
	}

	return listVertices;
}

bool CollisionDetection::OBBIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3* VolumeEdgeNormals = CalculateEdgeNormals(worldTransformA, worldTransformB); //Get axis of A and B
	Vector3* volAvertices = CalculateVerticesOBB(volumeA, worldTransformA); //Get vertices A
	Vector3* volBverices = CalculateVerticesOBB(volumeB, worldTransformB); // Get vertices B

	Vector3 SmallestOverlapAxis;
	float smallestOverlapMag = FLT_MAX;

	//Traverse through all axis of A & B
	for (int i = 0; i < 15; ++i) {
		if (VolumeEdgeNormals[i].Length() == 0) continue;
		float MinA = FLT_MAX;
		float MaxA = std::numeric_limits<float>::lowest();

		float MinB = FLT_MAX;
		float MaxB = std::numeric_limits<float>::lowest();
		//Traverse through all the vertices on Object A & B
		for (int j = 0; j < 8; ++j) {
			float ProjA = Vector3::Dot(VolumeEdgeNormals[i], volAvertices[j]) / VolumeEdgeNormals[i].Length();
			float ProjB = Vector3::Dot(VolumeEdgeNormals[i], volBverices[j]) / VolumeEdgeNormals[i].Length();


			MinA = std::min(ProjA, MinA);
			MaxA = std::max(ProjA, MaxA);

			MinB = std::min(ProjB, MinB);
			MaxB = std::max(ProjB, MaxB);
		}

		bool isOverlaping = (MinA <= MinB && MinB <= MaxA) ||
			(MinB <= MinA && MinA <= MaxB);
		if (!isOverlaping)
		{
			delete VolumeEdgeNormals;
			delete volAvertices;
			delete volBverices;
			return false;
		}

		//Check the minimum overlap
		if (std::min(MaxB, MaxA) - std::max(MinB, MinA) < smallestOverlapMag)
		{
			smallestOverlapMag = std::min(MaxB, MaxA) - std::max(MinB, MinA);
			SmallestOverlapAxis = VolumeEdgeNormals[i];
		}
	}


	collisionInfo.AddContactPoint(Vector3(worldTransformA.GetPosition()), Vector3(worldTransformB.GetPosition()), SmallestOverlapAxis, smallestOverlapMag);
	delete VolumeEdgeNormals;
	delete volAvertices;
	delete volBverices;
	return true;
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Matrix4 GenerateInverseProjection(float aspect, float nearPlane, float farPlane, float fov) {
	float negDepth = nearPlane - farPlane;

	float invNegDepth = negDepth / (2 * (farPlane * nearPlane));

	Matrix4 m;

	float h = 1.0f / tan(fov*PI_OVER_360);

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = invNegDepth;//// +PI_OVER_360;
	m.array[3][2] = -1.0f;
	m.array[3][3] = (0.5f / nearPlane) + (0.5f / farPlane);

	return m;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	Matrix4 proj  = cam.BuildProjectionMatrix(aspect);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = 1.0f / d;

	m.array[3][2] = 1.0f / e;
	m.array[3][3] = -c / (d * e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(pitch, Vector3(1, 0, 0));

	return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

