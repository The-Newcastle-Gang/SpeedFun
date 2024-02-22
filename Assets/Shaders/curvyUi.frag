#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;

void main() {

    float left = 0.2;
    float right = 0.8;

    float dist = distance(TexCoords, vec2(clamp(TexCoords.x, 0.1, 0.9), 0.5));
    float factor = step(0.1, dist);

    vec4 outgoingColor = uiColor;
    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }
    fragColor = vec4(outgoingColor);
}