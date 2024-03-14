#pragma once
#include <algorithm>
#include <vector>
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "OGLRenderer.h"
#include <glad/gl.h>
#define MAX_PARTICLES 1000

using namespace NCL;

//struct to define a particle
struct Particle {
	Vector3 position, speed;
	Vector4 colour;
	float size;
	float life;
	float cameraDistance;

	bool operator<(const Particle& other)const {
		return this->cameraDistance > other.cameraDistance;
	}
};


//particle system code adapted from https://github.com/opengl-tutorials/ogl/blob/master/tutorial18_billboards_and_particles/tutorial18_particles.cpp
namespace NCL
{
	namespace CSC8503
	{
		class ParticleSystem
		{
		public:
			ParticleSystem(Vector3 startPos, Vector3 rngLower, Vector3 rngHigher, int particlesPerBurst,
				float particleSpeed, float lifeSpan, float particleSize, float timeBetween, TextureBase* texture);
			~ParticleSystem();

			void SetShader(OGLShader* s) { shader = s; }
			void UpdateParticles(float dt, Vector3 cameraPos);
			void DrawParticles();
			void CreateNewParticles(float dt);
			TextureBase* GetTexture() { return texture; }
			int GetParticleCount() { return particlesCount; }
			Particle GetParticle() { return particles[0];}
			
		protected:
			TextureBase* texture;
			GLuint positionBuffer;
			GLuint vertexBuffer; //Used to send vertex positions of a square to shader
			GLfloat* positionData;
			OGLShader* shader;
			GLfloat vertices[12] = { -0.5f, -0.5f, 0.0f,0.5f, -0.5f, 0.0f,-0.5f, 0.5f, 0.0f,0.5f, 0.5f, 0.0f };

			Particle particles[MAX_PARTICLES] = {};
			int lastUsedParticle = 0;
			int particlesPerBurst;
			Vector3 startPos; //default position of particles
			Vector3 rngLower; //the lower bound for random particle placement
			Vector3 rngRange; //the range in which particles can be placed, from rngLower.

			int particlesCount;
			float lifeSpan;
			float particleSpeed;
			float particleSize;

			float timeBetween = 1.0f;

			float particleTimer = 0;

			int FindUnusedParticle();
			void SortParticles() { std::sort(&particles[0], &particles[MAX_PARTICLES]); }
		};
	}
}



