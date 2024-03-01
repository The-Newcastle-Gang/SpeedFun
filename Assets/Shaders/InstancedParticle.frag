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
	
	//vec3 viewDir = normalize ( cameraPos - IN . worldPos );
	//vec3 halfDir = normalize ( incident + viewDir );

	//float rFactor = max (0.0 , dot ( halfDir , IN.normal ));
	//float sFactor = pow ( rFactor , 80.0 );
	
	//vec4 albedo = IN.colour;
	
	//albedo *= texture(mainTex, IN.texCoord);

	//albedo.rgb = pow(albedo.rgb, vec3(2.2));

	
	//fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	//fragColor.a = albedo.a;

	fragColor = vec4(1, 0, 0, 1);

}