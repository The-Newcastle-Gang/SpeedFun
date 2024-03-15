#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform float uTime;

void main() {
    vec4 noiseTexture = vec4(1);
    vec2 uv= TexCoords;
    
    if (hasTexture == 1) {
        noiseTexture *= texture(tex, uv);
    }
    
    float dissolveValue = sin(uTime);
    float burnSize      = 0.04;
    vec4  burnColor     = vec4(0.84, 0.0, 0.34, 1.0);

    float burnStep = burnSize * step(0.001, dissolveValue) * step(dissolveValue, 0.999);
    float threshold = smoothstep( noiseTexture.x - burnStep,noiseTexture.x, dissolveValue);
    float border = smoothstep( noiseTexture.x, burnStep+noiseTexture.x, dissolveValue);

    vec4 backColor = vec4(0.0, 0.0, 0.0, 1.0);
    backColor.a *= threshold;
    backColor.rgb = mix(burnColor.rgb, backColor.rgb, border);

    fragColor = backColor;
}