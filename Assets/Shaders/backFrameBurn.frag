#version 330 core

#define PI 3.14159265

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform sampler2D noiseTexture;
uniform float uTime;



void main() {
    vec4 outgoingColor = uiColor;
    float radius = 0.4;
    vec2 dir = vec2(0.0,1.0);

    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }

    vec4 noiseTex = vec4(1.0);
    noiseTex *= texture(noiseTexture, TexCoords);

    vec2 uv = TexCoords;
    vec2 center2dist = vec2(0.5) - uv;

    float dist2Circle = length(center2dist) *radius;
    float dist2sqEdge =  0.5*(0.5 - min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y)));
    float distToEdge  = mix(dist2Circle, dist2sqEdge, 1.0);

    float gradient = smoothstep(0.5, 0.5 - radius, distToEdge);

    vec2 direction = vec2(0, 1) * 0.076;
    vec4 upNoise  = vec4(1.0);
    upNoise *= texture(noiseTexture, uv + dir * uTime * 0.05);
    float noiseVal = upNoise.r;

    float opacity = step(radius, mix(gradient, noiseVal, 0.309) - distToEdge);
    fragColor = vec4(outgoingColor.xyz, opacity) ;
}