#include "ParticleSystem.h"

using namespace NCL::CSC8503;

ParticleSystem::ParticleSystem(Vector3 startPos,Vector3 rngLower, Vector3 rngHigher,int particlesPerBurst,float particleSpeed,float lifeSpan,float particleSize, float timeBetweenBursts, TextureBase* texture) {
	lastUsedParticle = 0;
	this->texture = texture;
	this->particlesPerBurst = particlesPerBurst;
	this->particleSpeed = particleSpeed;
	this->lifeSpan = lifeSpan;
	this->particleSize = particleSize;
	this->startPos = startPos;
	this->rngLower = rngLower;
	this->timeBetween = timeBetweenBursts;
	this->rngRange = Vector3(rngHigher.x-rngLower.x, rngHigher.y - rngLower.y, rngHigher.z - rngLower.z) ;

    glGenVertexArrays(1, &particleVAO);
	//particles.resize(maxParticles);
    glBindVertexArray(particleVAO);
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * MAX_PARTICLES *sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);


	positionData = new GLfloat[MAX_PARTICLES * 4];
}

ParticleSystem::~ParticleSystem(){
	delete[] positionData;
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteBuffers(1,&positionBuffer);
    glDeleteVertexArrays(1, &particleVAO);
	delete texture;
}

void ParticleSystem::UpdateParticles(float dt, Vector3 cameraPos) {
	particlesCount = 0;

	for (int i = 0; i < MAX_PARTICLES; i++) {
		Particle& p = particles[i];
		if (p.life > 0.0f) {
			p.life -= dt;
			p.position += p.speed * dt;
			p.cameraDistance = Vector3(p.position - cameraPos).Length();

			positionData[4 * particlesCount] = p.position.x;
			positionData[4 * particlesCount +1] = p.position.y;
			positionData[4 * particlesCount+2] = p.position.z + sin((float)Window::GetTimer()->GetTotalTimeSeconds());
			positionData[4 * particlesCount+3] = p.size;

		}
		else {
			p.cameraDistance = -1.0f;
			positionData[4 * particlesCount] = -1000;
			positionData[4 * particlesCount + 1] = -1000;
			positionData[4 * particlesCount + 2] = -1000;
			positionData[4 * particlesCount + 3] = p.size;
		}
		particlesCount++;
	}
	SortParticles();
}

void ParticleSystem::CreateNewParticles(float dt) {
	particleTimer += dt;
	int newParticles = 0;
	if (particleTimer >= timeBetween)
	{
		particleTimer = 0;
		newParticles = particlesPerBurst;
	}

	for (int i = 0; i < newParticles; i++) {
		int particleIndex = FindUnusedParticle();
		particles[particleIndex].life = 0.01f*(rand() % 20)+lifeSpan;
		particles[particleIndex].position = startPos + 
			Vector3(rngLower.x + rand()%(int)rngRange.x,
				rngLower.y + rand() % (int)rngRange.y,
				rngLower.z + rand() % (int)rngRange.z);
		particles[particleIndex].speed = Vector3(0, particleSpeed, 0);
		particles[particleIndex].size = particleSize;
	}
}

int ParticleSystem::FindUnusedParticle() {
	for (int i = lastUsedParticle; i < MAX_PARTICLES; i++) {
		if (particles[i].life <= 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) {
		if (particles[i].life <= 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	return 0;
}
void ParticleSystem::DrawParticles() {
    glBindVertexArray(particleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, positionData);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);

}