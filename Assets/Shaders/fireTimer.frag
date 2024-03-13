#version 330 core

#define PI 3.14159265

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform sampler2D tex;
uniform sampler2D noiseTexture;
uniform sampler2D cheeseTexture;
uniform float uTime;


vec4 uStep (vec4 edge, vec4 inval){
    return step(edge, inval);
}

void main() {
    vec4 outgoingColor = vec4(0);

    vec2 uv= TexCoords;
    vec2 Noise1uv = vec2(uv.x, uv.y + uTime * 0.6);
    vec4 noise1 = texture(cheeseTexture, Noise1uv);

    vec2 Noise2uv = vec2(uv.x, uv.y  + uTime *0.6);
    vec4 noise2 = texture(cheeseTexture, Noise2uv* 0.5);

    vec4 finalnoise = noise1 * noise2;

    
    vec4 mask;
    if (hasTexture == 1) {
        mask = texture(tex, TexCoords);
    }
    vec4 greyscaleFlame = finalnoise + mask;
    greyscaleFlame *=mask;

    vec4 stepInner = step(0.4, greyscaleFlame);
    vec4 stepOpacity  = step(0.1, greyscaleFlame);
    

    //#CB1143
    vec4 outerColor = vec4(0.796,0.067,0.263, 1);
    vec4 outerFlame = outerColor * (1.0-stepInner);

    vec4 innerColor = vec4(1.,0.51,0.059,1);
    vec4 innerFlame = innerColor * stepInner;

    vec4 finalFlame = outerFlame+ innerFlame;

    fragColor = vec4(finalFlame.rgb, stepOpacity);
 }