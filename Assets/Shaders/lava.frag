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

vec4 uLerp( vec4 a, vec4 b, vec4 t){
  return mix(a,b, t);
}

float random(in vec2 uv){
  return fract(sin(dot(uv.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}


void main(void)
{
	vec2 uv = IN.texCoord;
	vec2 uvOriginal = uv * 25;
  uv *= 100;
  vec3 col = vec3(0.0, 0.0, 0.0);

  float distortScale = 0.37;
  vec2 velocity      = 0.1 * vec2(-0.444 * u_time, -0.9 * u_time);

  vec2 firstNoise = distortScale * velocity + (uvOriginal);
  vec2 distortedTexCoords = vec2(uvOriginal.x * fbm(firstNoise  + 0.0001* u_time),uvOriginal.y  * fbm(firstNoise  + 0.002* u_time));

  
  col += fbm(firstNoise);
  col += fbm(firstNoise + 0.5) * 0.5;
  col = 1.0-col;
  
  
  vec4 noiseyTexture = vec4(col, 1.0) +  texture(mainTex, distortedTexCoords);
  float tintOffset      = 1.0;
  vec4 tintColorStart   =  vec4(0.99, 0.35, 0.0, 1.0);
  vec4 tintColorEnd     =  vec4(0.99, 0.0, 0.0, 1.0);
  
  float brightness = 1.0;
  vec4 newLavaCol = uLerp(tintColorStart, tintColorEnd, tintOffset* noiseyTexture);
  
  fragColor =  newLavaCol * brightness + texture(mainTex, distortedTexCoords) * newLavaCol;


  
  // fragColor = noiseyTexture;

}