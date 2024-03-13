#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseNorm;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D rayMarchColor;
uniform sampler2D rayMarchNorm;


in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void){
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	float isLit = texture(diffuseNorm, IN.texCoord).w;
	vec3 light = texture(diffuseLight,IN.texCoord).xyz;
	vec3 specular = texture(specularLight, IN.texCoord).xyz;
	vec4 rayColor = texture(rayMarchColor, IN.texCoord).xyzw;

	isLit = 1; 
	fragColour.xyz = diffuse;
	fragColour.xyz += light * isLit;
	fragColour.xyz += specular * isLit;
	fragColour.a = 1.0;
}