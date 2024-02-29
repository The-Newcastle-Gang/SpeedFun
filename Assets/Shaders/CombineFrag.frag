#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseNorm;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;


in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void){
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	//float isLit = texture(diffuseNorm, IN.texCoord).w;
	//vec3 light = texture(diffuseLight,IN.texCoord).xyz;
	//vec3 specular = texture(specularLight, IN.texCoord).xyz;

	//isLit = 0; 
	//fragColour.xyz = diffuse * 0.1 * isLit + diffuse * (1-isLit);
	//fragColour.xyz += diffuse * light* isLit;
	//fragColour.xyz += specular* isLit;
	//fragColour.a = 1.0;
	
	fragColour.xyz = diffuse;
	fragColour.a = 1.0;
}