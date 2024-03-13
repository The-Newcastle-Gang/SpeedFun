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

    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }

    vec2 adjustedUV = vec2(TexCoords.x, TexCoords.y * 0.2 + uTime * 0.1);
    float noiseVal = texture(noiseTexture, adjustedUV).x;
    float gradientVal = TexCoords.y;
    float combined = step(noiseVal, gradientVal);


    fragColor = vec4(outgoingColor.rgb * combined, outgoingColor.a);
}