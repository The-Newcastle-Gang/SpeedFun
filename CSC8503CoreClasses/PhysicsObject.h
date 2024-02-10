#pragma once
using namespace NCL::Maths;
#include <Matrix3.h>

namespace NCL {
	class CollisionVolume;
	
	namespace CSC8503 {
		class Transform;

        struct PhysicsMaterial {
            float e = 0.8f;
            float linearDampingVertical = 0.995f;
            float linearDampingHorizontal = 0.995f;
            float angularDamping = 0.995f;
            PhysicsMaterial() {
		e = 0.8f
                linearDampingVertical = 0.995f;
                linearDampingHorizontal = 0.995f;
                angularDamping = 0.995f;
            }
            PhysicsMaterial(float coeffOfRest, float linV, float linH, float ang) {
		e = coeffOfRest;
                linearDampingVertical = linV;
                linearDampingHorizontal = linH;
                angularDamping = ang;
            }
        };

		class PhysicsObject	{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume, PhysicsMaterial* physMat);
			~PhysicsObject();

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void InitCubeInertia();
			void InitSphereInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

			float GetElasticity() const {
				return physicsMaterial->e;
			}

            float GetLinearDampVertical() const {
                return physicsMaterial->linearDampingVertical;
            }

            float GetLinearDampHorizontal() const {
                return physicsMaterial->linearDampingHorizontal;
            }
            float GetAngularDamp() const {
                return physicsMaterial->angularDamping;
            }
            PhysicsMaterial* GetPhysMat() {
                return physicsMaterial;
            }
            void SetPhysMat(PhysicsMaterial* physMat) {
                physicsMaterial = physMat;
            }

		protected:
			const CollisionVolume* volume;
			Transform*		transform;

			float inverseMass;
            PhysicsMaterial* physicsMaterial;

			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;
			
			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;
		};
	}
}

