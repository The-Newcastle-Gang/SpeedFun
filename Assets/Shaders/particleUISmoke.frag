#version 330 core

#define PI 3.14159265

out vec4 fragColor;
in vec2 TexCoords;

uniform int hasTexture;
uniform sampler2D tex;
uniform sampler2D noiseTexture;
uniform float uTime;



void main() {
    vec4 outgoingColor = vec4(0.7059, 0.7059, 0.7059, 1.0);

    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }

    vec2 adjustedUV = vec2(TexCoords.x, TexCoords.y * 1.2 + uTime/4 * 0.1);
    float noiseVal = texture(noiseTexture, adjustedUV).x;
    float gradientVal = TexCoords.y - 0.5;
    float combined = step(noiseVal, gradientVal);


    fragColor = vec4(outgoingColor.rgb * combined, outgoingColor.a);
}