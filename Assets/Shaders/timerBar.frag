#version 330 core

out vec4 fragColor;
in vec2 TexCoords;

uniform vec4 uiColor;
uniform int hasTexture;
uniform float uTime;
uniform sampler2D tex;

float random(in vec2 uv){
  return fract(sin(dot(uv.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise (in vec2 uv) {
    vec2 i = floor(uv);
    vec2 f = fract(uv);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define OCTAVES 6

float fbm (in vec2 uv) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(uv);
        uv *= 2.;
        amplitude *= .5;
    }
    return value;
}


void main() {
    
    vec4 outgoingColor = uiColor;
    if (hasTexture == 1) {
        outgoingColor *= texture(tex, TexCoords);
    }
    
    vec2 uv = TexCoords;
    uv.x*= 20;
    uv.x += uTime;
    vec3 color = vec3(1,1,1);
    
    vec2 q = vec2(0.);
    q.y = fbm( uv + 0.4*uTime);
    q.x = fbm( uv + vec2(1.0));

    vec2 r = vec2(0.);
    r.x = fbm( uv + 1.0*q + vec2(1.7,9.2)- 0.15*uTime );
    r.y = fbm( uv + 1.0*q + vec2(8.3,2.8)- 0.126*uTime);

    float f = fbm(uv);

    color = mix(vec3(0.3608, 0.3608, 0.3608),
                vec3(0.0, 0.0, 0.0),
                clamp((f*f)*4.0,0.0,1.0));

    color = mix(color,
                vec3(0,0,0.164706),
                clamp(length(f),0.0,1.0));

    color = mix(color,
                outgoingColor.rgb,
                clamp(length(r.x),0.0,1.0));

    int colourThreasholds = 8;
    color = round(color * colourThreasholds) / colourThreasholds;
    // fragColor = outgoingColor * sin(uTime);
    fragColor = vec4(color, 1);
}