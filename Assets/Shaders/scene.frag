#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	float shadow = 1.0; // New !
	
	if( IN . shadowProj . w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj, 1.0) * 0.5f;
	}

	vec3  incident = normalize ( lightPos - IN.worldPos );
	float lambert  = max (0.0 , dot ( incident , IN.normal )) * 0.9; 
	
	vec3 viewDir = normalize ( cameraPos - IN . worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	float rFactor = max (0.0 , dot ( halfDir , IN.normal ));
	float sFactor = pow ( rFactor , 80.0 );
	
	vec4 albedo = IN.colour;
	
	if(hasTexture) {
	 albedo *= texture(mainTex, IN.texCoord);
	}
	
	albedo.rgb = pow(albedo.rgb, vec3(2.2));
	
	fragColor.rgb = albedo.rgb * 0.05f; //ambient

	float toonLambert = smoothstep(0.0, 0.01, dot(normalize(lightPos - IN.worldPos), IN.normal));
	float toonSpecular = smoothstep(0.155, 0.16, sFactor);
	
	fragColor.rgb += albedo.rgb * lightColour.rgb * toonLambert;// * shadow; //diffuse light
	
	fragColor.rgb += lightColour.rgb * toonSpecular;// * shadow; //specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	fragColor.a = albedo.a;

//fragColor.rgb = IN.normal;

	//fragColor = IN.colour;
	
	//fragColor.xy = IN.texCoord.xy;
	
	//fragColor = IN.colour;
}