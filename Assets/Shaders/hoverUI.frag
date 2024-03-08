#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform float tweenValue1;

vec4 getBarColor(vec4 inputColor) {
    float factor = inputColor.r * step(distance(TexCoords.x, 0.5), tweenValue1 * 0.5);
    return factor * vec4(vec3(1.0), inputColor.a);
}

vec4 getDiamondColor(vec4 inputColor) {
    float factor = inputColor.g;
    return factor * vec4(vec3(1.0), inputColor.a);
}

void main() {
    vec4 outgoingColor = texture(tex, TexCoords);

    fragColor = vec4(getBarColor(outgoingColor) + getDiamondColor(outgoingColor));
}