#version 400 core

uniform sampler2D 	mainTex;
uniform vec3	cameraPos;

in Vertex
{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

float vignette(float value, vec2 uv)
{
	uv *= 1.0 - uv.yx;
	float vig = uv.x * uv.y * 20;
	vig = pow(vig, value * 20);
	return vig;
}

void main(void)
{
	vec2 uv = IN.texCoord;
	vec3 col = vec3(1.0, 0.0, 0.8314);
	fragColor = vec4(col, 1.0 * vignette(0.999, uv));
}