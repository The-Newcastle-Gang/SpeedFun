#version 330 core
layout (location = 0) in vec4 vertex; // 2d coords, and 2d texcoords

out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 sizeRel;
uniform vec2 sizeAbs;
uniform vec2 positionRel;
uniform vec2 positionAbs;

uniform float uTime;

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
  float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

void main()
{
    vec2 newVertex;
    newVertex = vertex.xy * sizeAbs;
    newVertex = newVertex.xy + vertex.xy * sizeRel;
    newVertex.x = newVertex.x + positionRel.x + positionAbs.x;
    newVertex.y = newVertex.y + positionRel.y + positionAbs.y;

    vec4 finalPos = projection * vec4(newVertex.xy, 0.0, 1.0);

    mat4 tmat = mat4(
        1,  sin(noise(noise(uTime))), 0,0,
        0, 1, 0,0,
        0, 0, 1,0,
        0,0,0,1
    );
    finalPos *=tmat;

    gl_Position= finalPos;
    TexCoords = vertex.zw;
}