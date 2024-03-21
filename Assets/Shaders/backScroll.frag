#version 330 core

#define PI 3.14159265

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform sampler2D noiseTexture;
uniform float uTime;

vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, s, -s, c);
	return m * v;
}

vec2 movingTiles(vec2 st, float freq, float velocity){
    st *= freq;
    float time = uTime*velocity;
    st.x += fract(time)*2.0;
    return fract(st);
}

float box(vec2 st, vec2 boxSize, float edgeSmoothing){
    boxSize = vec2(0.5)-boxSize*0.5;
    vec2 suv = vec2(edgeSmoothing*0.5);
    vec2 uv = smoothstep(boxSize,boxSize+suv,st);
    uv *= smoothstep(boxSize,boxSize+suv,vec2(1.0)-st);
    return uv.x*uv.y;
}

float gradient(vec2 st){
  return (st.x + st.y)*0.5;
}

void main() {
    vec4 outgoingColor = uiColor;

    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }
    
    vec3 color = vec3(0.5);
    vec2 uv = TexCoords;
    uv = rotate(uv, -45.0);
    color *= gradient(uv);
    uv = movingTiles(uv,20.,0.5);
    color *= vec3(box(uv, vec2(0.5), 0.01));
    fragColor = vec4(color, 1.0);
}