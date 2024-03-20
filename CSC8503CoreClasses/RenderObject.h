#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"
#include "MeshAnimation.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "AnimatorObject.h"

namespace NCL {
	using namespace NCL::Rendering;
	class MeshGeometry;
	namespace CSC8503 {
		class Transform;
        
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

            void SetMeshScale(Vector3 scale) { meshScale = scale; }
            Vector3 GetMeshScale() const { return meshScale; }

            void SetMeshOffset(Vector3 Offset) { meshOffset = Offset; }
            Vector3 GetMeshOffset() const { return meshOffset; }

            void SetMeshMaterial(MeshMaterial* m) {
                meshMaterial = m;
            }

            MeshMaterial* GetMeshMaterial() const {
                return meshMaterial;
            }

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

            void SetAnimatorObject(AnimatorObject* a) {
                animatorObject = a;
            }
            AnimatorObject* GetAnimatorObject() const {
                return animatorObject;
            }

			void SetDepthTest(bool b) {
				isDepthTestRendered = b;
			}

			bool IsDepthTested() const {
				return isDepthTestRendered;
			}

		protected:
			MeshGeometry*	mesh;
            MeshMaterial* meshMaterial;
			TextureBase*	texture;
			ShaderBase*		shader;
			Transform*		transform;
            AnimatorObject* animatorObject;
			Vector4			colour;
            Vector3         meshScale;
            Vector3         meshOffset;
			bool			isDepthTestRendered;
		};
	}
}
