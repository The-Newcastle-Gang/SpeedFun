#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform bool isLit;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour[3];

void main(void)	{
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	vec3 bumpNormal = texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;
	bumpNormal = normalize(TBN * normalize(bumpNormal));

	fragColour[0] = texture2D(diffuseTex, IN.texCoord) * IN.colour;
	fragColour[1] = vec4(bumpNormal.xyz * 0.5 + 0.5, 1.0);
	fragColour[1].w = isLit?1.0:0.0;

	fragColour[2] = vec4(0,0,0,1);

	//fragColour[0] = vec4(fragColour[1].w,fragColour[1].w,fragColour[1].w,1.0);
}