#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;

float sdRoundBox( in vec2 p, in vec2 b, in vec4 r )
{
    r.xy = (p.x>0.0)?r.xy : r.zw;
    r.x  = (p.y>0.0)?r.x  : r.y;
    vec2 q = abs(p)-b+r.x;
    return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r.x;
}



void main() {

    float left = 0.2;
    float right = 0.8;

    float dist = distance(TexCoords, vec2(clamp(TexCoords.x, 0.1, 0.9), 0.5));
    float factor = step(0.1, dist);

    vec4 outgoingColor = uiColor;
    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }
    fragColor = vec4(outgoingColor.xyz * sdCircle(TexCoords + vec2(-0.5, -0.5), vec2(0.0, 0.0)), 1.0);
}