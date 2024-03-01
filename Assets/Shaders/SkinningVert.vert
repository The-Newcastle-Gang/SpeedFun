#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowMatrix;
uniform mat4 textureMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in ivec4 jointIndices;

uniform vec4 		objectColour = vec4(1,1,1,1);

uniform mat4 	joints[110];

uniform mat4 	nextJoints[110];

uniform float frameLerp;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} OUT;

void main(void) {
	vec4 localNormal = vec4(normal, 1.0f);
	vec4 localPos 	= vec4(position, 1.0f);
	vec4 skelPos 	= vec4(0,0,0,0);
	vec4 nextSkelPos = vec4(0,0,0,0);

	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));

	for(int i = 0; i < 4; ++i) {
		int   jointIndex 	= jointIndices[i];
		float jointWeight 	= jointWeights[i];

		skelPos += joints[jointIndex] * localPos * jointWeight;
		nextSkelPos += nextJoints[jointIndex] * localPos * jointWeight;
	}

	vec4 lerpedPos = mix(skelPos,nextSkelPos,frameLerp);

	OUT.shadowProj 	=  shadowMatrix * vec4 ( position,1);
	OUT.worldPos 	= ( modelMatrix * vec4 ( position ,1)). xyz;
	OUT.normal = normalize ( normalMatrix * normalize ( normal ));
	OUT.colour = objectColour;

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(lerpedPos.xyz, 1.0);
	OUT.texCoord = texCoord;

}