#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;

uniform float u_time;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

float vignette(float value, vec2 uv){
	uv *= 1.0-uv.yx;
    float vig = uv.x*uv.y *20.5;
    vig = pow(vig, value *2.0);
    return vig;
}



void main(void)
{
	vec2 uv = IN.texCoord;
	vec2 uvOriginal = IN.texCoord;
	uv *=3.;

	vec3 col = vec3(0.);
	vec2 currentGridID = floor(uv);
	vec2 currentGridCoord = fract(uv);

	float minDistFromPixel = 1.0f;

	for(float i= -1.0; i<= 1.0; i++){
    for(float j = -1.0; j<=1.0; j ++){

    vec2 neighbor = vec2(i,j); 
    vec2 neighborpoint = neighbor; //this right now because were assuming the adjacent is the center

    // vec2 noise = random2(currentGridId +neighbor);
    // neighborpoint = neighbor + sin(u_time * noise) * 0.5;

    float dist = length(currentGridCoord - neighborpoint);
    minDistFromPixel = min(dist, minDistFromPixel);
    }
  }

  col = vec3(smoothstep(0., 0.9, 1.0f -minDistFromPixel));
  col*= vignette(1.0, uvOriginal);
  fragColor = vec4(col, 1.0);

}