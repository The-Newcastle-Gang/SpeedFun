#version 330 core

layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs;

out Vertex{
	vec2 texCoord;
	vec4 colour;
} OUT;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	float particleSize = xyzs.w;
	vec3 particleCenter = xyzs.xyz;
	vec3 vertexPosition = particleCenter + squareVertices.xyz;
	mat3 particleMatrix = mat3(particleSize);
	mat4 modelMatrix = mat4(particleMatrix);
	modelMatrix[3][0] = particleCenter.x;
	modelMatrix[3][1] = particleCenter.y;
	modelMatrix[3][2] = particleCenter.z;

	mat4 viewModel = viewMatrix * modelMatrix;

	viewModel[0][0] = particleSize;
	viewModel[1][1] = particleSize;
	viewModel[2][2] = particleSize;
	
	viewModel[0][1] = 0.0;
	viewModel[0][2] = 0.0;
	viewModel[1][0] = 0.0;
	viewModel[1][2] = 0.0;
	viewModel[2][0] = 0.0;
	viewModel[2][1] = 0.0;

	mat4 mvp = projMatrix * viewModel;

	// Output position of the vertex
	gl_Position = mvp * vec4(squareVertices,1.0);

	// UV of the vertex. No special space for this one.
	OUT.texCoord = squareVertices.xy + vec2(0.5, 0.5);
	OUT.texCoord.y *= -1;
	OUT.colour = vec4(1,1,1,1);
}