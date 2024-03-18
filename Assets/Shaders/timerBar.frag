#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform float tweenValue1;

void main() {
    vec4 outgoingColor = uiColor;
    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }

    vec4 progressModifier = vec4(vec3(1.0), 1 - step(tweenValue1, TexCoords.x));

    fragColor = outgoingColor * progressModifier;
}