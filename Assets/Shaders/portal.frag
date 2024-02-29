#version 400 core

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

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec2 twirl(float amount, vec2 uv,  float t2){
    float angle = atan(uv.y, uv.x );
    float radius= length(uv);
    angle += radius * amount;
    vec2 shifted = radius * vec2(cos(angle), sin(angle));
    return shifted;
}

vec2 rotTwo(float amount, vec2 uv){
    mat2 rot_mat = mat2(    vec2(sin(amount), -cos(amount)),
                            vec2(cos(amount), sin(amount)));
    return uv*rot_mat;
}

void main(void)
{
	vec2 uv = IN.texCoord;
	vec2 uvOriginal = IN.texCoord;
	uv *=3.;
	uv= twirl (5.0  + (sin(u_time)), uv-1.5, u_time);
    uv = rotTwo(u_time, uv);

	vec3 col = vec3(0.);
	vec2 currentGridID = floor(uv);
	vec2 currentGridCoord = fract(uv);

	float minDistFromPixel = 1.0f;

	for(float i= -1.0; i<= 1.0; i++){
    	for(float j = -1.0; j<=1.0; j ++){

    		vec2 neighbor = vec2(i,j); 
    		vec2 neighborpoint = neighbor; 
			vec2 noise = random2(currentGridID +neighbor);
    		neighborpoint = neighbor + sin(u_time * noise) * 0.5;

    		float dist = length(currentGridCoord - neighborpoint);
    		minDistFromPixel = min(dist, minDistFromPixel);
    	}
  	}

  vec3 random =  vec3(random2(uvOriginal), 1.0);
  col = vec3(smoothstep(0., 0.9, 1.0-minDistFromPixel));
  col *= vignette(1.0, uvOriginal);
  col *= vec3(1.0, 0.5*sin(u_time), 0.0);
  fragColor = vec4(col, smoothstep(0.01, 0.05, length(col)));

}