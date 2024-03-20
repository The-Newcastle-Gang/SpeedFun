#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform sampler2D noiseTexture;
uniform float tweenValue1;
uniform float uTime;


void main() {
    vec4 outgoingColor = uiColor;
    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }

    float fireAlpha = 1 - step(tweenValue1 + 0.1, TexCoords.x);
    float progressAlpha = 1 - step(tweenValue1, TexCoords.x);
    fireAlpha -= progressAlpha;

    vec2 adjustedUV = vec2(TexCoords.x * 10 + uTime * 2, TexCoords.y);
    float noiseVal = texture(noiseTexture, adjustedUV).x;
    float gradientVal = 1 - smoothstep(tweenValue1,tweenValue1 + 0.1, TexCoords.x);
    float combined = step(noiseVal, gradientVal);
    fireAlpha *= combined;

    fragColor = outgoingColor * vec4(vec3(1.0), fireAlpha) + outgoingColor * vec4(vec3(1.0), progressAlpha);
}