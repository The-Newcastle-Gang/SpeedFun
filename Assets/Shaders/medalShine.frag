#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform float uTime;

void main() {
    vec4 outgoingColor = uiColor;
    vec2 uv            = TexCoords;
    vec4 shineColor    = vec4(1.0, 1.0, 1.0 ,0.5);
    float shineSpeed   = 4.0;
    float shineSize    = 0.01;

    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }
    
    float shine = step(1.0 - shineSize * 0.5, 0.5 + 0.5 * sin(uv.x - uv.y + uTime * shineSpeed));
    fragColor = outgoingColor;
    fragColor.xyz = mix(fragColor.xyz, shineColor.xyz, shine * shineColor.a );
    
    
}