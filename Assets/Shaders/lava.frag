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

float random(in vec2 uv){
  return fract(sin(dot(uv.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise(in vec2 st){
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f*f*(3.0-2.0*f);
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main(void)
{
	vec2 uv = IN.texCoord;
	vec2 uvOriginal = uv;

	 uv += u_time *0.1;
  vec3 col = vec3(1.0, 0.0, 0.0);

  uv.x += sin(uv.y * 9.0 + u_time * 0.5)/10.0;
  uv.y += cos(uv.x * 5.0 + u_time *0.02)/7.0;

  vec2 p = vec2(uv * (5.+3.*sin(u_time * 0.0005)));
  vec2 q = vec2(uv * 5.0);
  float n = noise(p) * noise(q) ;

  col = vec3(n) * 6.9 ;
  // col.x = pow(col.x, 1.5);
  
  // col.x = pow(col.x, 2.0);

  // col = vec3(0.6353, 0.6353, 0.6353) + diff;

  // fragColor = vec4(col, 1.0);
  fragColor = texture(mainTex, IN.texCoord) + vec4(col, 1.0);
}