#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform mat4 inverseProjView;

out vec4 diffuseOutput;
out vec4 specularOutput;

void main(void)	{
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texture(depthTex, texCoord.xy).r;
	vec3 ndcPos = vec3(texCoord, depth) * 2.0 -1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz/invClipPos.w;

	float dist = length(lightPos - worldPos);
	float atten = 1 - step(0.0, dist - lightRadius);

	if(atten==0.0)discard;

	vec3 normal = texture(normTex,texCoord.xy).xyz;
	vec3 incident = normalize(lightPos - worldPos);
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float lambert = dot(incident, normal);
//	float specFactor = clamp(dot(halfDir,normal),0.0,1.0);
//	specFactor = pow(specFactor,60.0);

	vec3 attenuated = lightColour.xyz * atten;


	diffuseOutput = vec4(attenuated,1.0) ; //supposed to be * lambert but looks funny
	specularOutput = vec4(vec3(0.0),1.0);
}