#version 330 core

uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;
uniform sampler2D bumpTex;
uniform bool isLit;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	
} IN;

out vec4 fragColour[2];

void main(void)	{
	float shadow = 1.0; // New !

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
	
	vec3 bumpNormal = texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;
	bumpNormal = normalize(TBN * normalize(bumpNormal));

	fragColour[1] = vec4(bumpNormal.xyz * 0.5 + 0.5, 1.0);
	fragColour[1].w = isLit?1.0:0.0;

	if( IN . shadowProj . w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
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
	
	fragColour[0].rgb = albedo.rgb * 0.05f; //ambient
	
	fragColour[0].rgb += albedo.rgb * lightColour.rgb * lambert * shadow; //diffuse light
	
	fragColour[0].rgb += lightColour.rgb * sFactor * shadow; //specular light
	
	fragColour[0].rgb = pow(fragColour[0].rgb, vec3(1.0 / 2.2f));
	
	fragColour[0].a = albedo.a;

	


}