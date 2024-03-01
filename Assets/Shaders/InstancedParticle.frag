#version 400 core

uniform sampler2D 	mainTex;
uniform vec3	cameraPos;

in Vertex
{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void)
{

	
	fragColor = vec4(0.902, 0.3451, 0.7608, 0.733);

}