#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;

void main() {
    vec4 outgoingColor = uiColor;
    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }
    fragColor = outgoingColor;
}