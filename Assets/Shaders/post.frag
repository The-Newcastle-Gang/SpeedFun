#version 330

out vec4 fragColor;
uniform sampler2D hdrBuffer;
uniform sampler2D depthBuffer;
uniform mat4 invViewPersp;
uniform vec3 lightPos;

uniform float u_time;
uniform bool SpeedLinesActive;
uniform float speedLineAmount;
uniform int speedLineDir;

#define M_PI 3.1415926535897932384626433832795
#define OCTAVES 6
#define LINECOUNT 20.0

in Vertex {
    vec2 texCoord;
} IN;

float distance_from_sphere(in vec3 p, in vec3 c, float r)
{
    return length(p - c) - r;
}

float smoothMax(float a, float b, float k) {
    return log(exp(k * a) + exp(k * b)) / k;
}

float smoothMin(float a, float b, float k) {
    return -smoothMax(-a, -b, k);
}

float map_the_world(in vec3 p)
{
    float sphere_0 = distance_from_sphere(p, vec3(0.0, 0.0, 3.0), 1.0);

    return sphere_0;
}

float linearDepth(float depth) {
    float near = 0.1f;
    float far = 500.0f;
    float ndc = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - ndc * (far - near));
}

vec3 calculate_normal(in vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

    float gradient_x = map_the_world(p + small_step.xyy) - map_the_world(p - small_step.xyy);
    float gradient_y = map_the_world(p + small_step.yxy) - map_the_world(p - small_step.yxy);
    float gradient_z = map_the_world(p + small_step.yyx) - map_the_world(p - small_step.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

    return normalize(normal);
}

vec4 ray_march(in vec3 ro, in vec3 rd)
{
    float total_distance_traveled = 0.0;
    const int NUMBER_OF_STEPS = 32;
    const float MINIMUM_HIT_DISTANCE = 0.001;
    const float MAXIMUM_TRACE_DISTANCE = 500.0;

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        vec3 current_position = ro + total_distance_traveled * rd;

        float distance_to_closest = map_the_world(current_position);

        float depth = (total_distance_traveled - 0.1) / (500.0f - 0.1);
        float worldDepth = linearDepth(texture(depthBuffer, IN.texCoord).r) / 500.0f;

        if (depth > worldDepth) {
            break;
        }

        if (distance_to_closest < MINIMUM_HIT_DISTANCE)
        {
            vec3 normal = calculate_normal(current_position) * -1;
            vec3 direction_to_light = normalize(current_position - lightPos);

            float diffuse_intensity = max(0.0, dot(normal, direction_to_light));
            float ambient = 0.2;

            return vec4(vec3(1.0, 0.5, 0.2) * diffuse_intensity * 0.8 + vec3(1.0, 0.5, 0.2) * ambient, 1.0);
        }

        if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE)
        {
            break;
        }
        total_distance_traveled += distance_to_closest;
    }
    return vec4(0.0);
}

vec3 unproject(vec3 position) {
    vec4 clipSpace = vec4(position.x, position.y, position.z, 1.0f);
    vec4 transformed = invViewPersp * clipSpace;
    transformed.x /= transformed.w;
    transformed.y /= transformed.w;
    transformed.z /= transformed.w;
    return transformed.xyz;
}

mat3 horizontal = mat3(
    1.0, 2.0, 1.0,
    0.0, 0.0, 0.0,
    -1.0, -2.0, -1.0
);

mat3 vertical = mat3(
    1.0, 0.0, -1.0,
    2.0, 0.0, -2.0,
    1.0, 0.0, -1.0
);


void samplePoints(inout float n[9], vec2 uv, float thickness)
{
    float w = 1.0 * thickness;
    float h = 1.0 * thickness;
    n[0] = linearDepth(texture(depthBuffer, uv + vec2( -w, -h)).r) * 0.002;
    n[1] = linearDepth(texture(depthBuffer, uv + vec2(0.0, -h)).r) * 0.002;
    n[2] = linearDepth(texture(depthBuffer, uv + vec2(  w, -h)).r) * 0.002;
    n[3] = linearDepth(texture(depthBuffer, uv + vec2( -w, 0.0)).r) * 0.002;
    n[4] = linearDepth(texture(depthBuffer, uv + vec2(0.0, 0.0)).r) * 0.002;
    n[5] = linearDepth(texture(depthBuffer, uv + vec2(  w, 0.0)).r) * 0.002;
    n[6] = linearDepth(texture(depthBuffer, uv + vec2( -w, h)).r) * 0.002;
    n[7] = linearDepth(texture(depthBuffer, uv + vec2(0.0, h)).r) * 0.002;
    n[8] = linearDepth(texture(depthBuffer, uv + vec2(  w, h)).r) * 0.002;
}

float sobelResult(vec2 uv, float thickness) {
    float points[9];
    samplePoints(points, uv, thickness);
    float horizontalEdge = points[2] + (2.0*points[5]) + points[8] - (points[0] + (2.0*points[3]) + points[6]);
    float verticalEdge = points[0] + (2.0*points[1]) + points[2] - (points[6] + (2.0*points[7]) + points[8]);
    return length(vec2(horizontalEdge, verticalEdge));
}

float correctDepthForThreshold(float x) {
    return min(1, 1 - cos(x*M_PI*2));
}

// =========== SPEEDLINES ========= //

float random(in vec2 uv){
  return fract(sin(dot(uv.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float fbm (in vec2 st) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

vec2 toPolar(vec2 cartesian, float center, float RadialScale, float LengthScale){

  float distance = length(cartesian);
  // float angle = atan(cartesian.y, cartesian.x);

  vec2 delta = cartesian -vec2(center);
  float radius = length(delta) * 2.0* RadialScale;
  float angle = atan(delta.x, delta.y) * 1.0/6.28 * LengthScale;
  return  vec2(radius, angle);

}

float dist(vec2 uv, vec2 center){
  return distance(uv, center);
}

vec3 inverseLerp(float a, float b, vec3 T){
  return (T-a)/(b-a);
}

vec2 uRotate(vec2 uv, float amount, float center){
    uv-=center;
    amount *= M_PI/180.0;
    float s = sin(amount);
    float c = cos(amount);
    mat2 rmat = mat2(c, -s, s, c);
    rmat *= 0.5;
    rmat += 0.5;
    rmat = rmat * 2. -1.;
    uv.xy = uv.xy *rmat;
    uv+=center;
    return uv;
}



// ============ MAIN =============

void main() {

    // 1 / 500 (far plane) = 0.002;
    float far = 0.002;
    float threshold = 0.003;
    float thickness = 0.001;
    float tighten = 30;
    float strength = 5.0;
    float depthThreshold = 0.4;

//    vec2 uv = IN.texCoord.xy * 2.0 - 1.0;

    vec4 hdrColor = texture(hdrBuffer, IN.texCoord).rgba;
    float worldDepth = linearDepth(texture(depthBuffer, IN.texCoord).r) * far;
//    vec3 rayOrigin = unproject(vec3(uv.x, uv.y, -1.0f));
//    vec3 rayDirection = normalize(unproject(vec3(uv.x, uv.y, 1.0f)) - rayOrigin);
//    vec4 raySphere = ray_march(rayOrigin, rayDirection);

    threshold += correctDepthForThreshold(worldDepth) * depthThreshold;

    float border = sobelResult(IN.texCoord, thickness);
    border = smoothstep(0, threshold, border);
    border = pow(border, tighten);
    border *= strength;

//
//    fragColor = vec4(raySphere.xyz + hdrColor.xyz * (1 - raySphere.a), raySphere.a + hdrColor.a);

    fragColor = vec4(hdrColor.rgb - border, step(0.99, border) + hdrColor.a);



    //=========SPEEDLINES============

    float speed     = 0.1;
    float velocity  = u_time* speed;
    float frames    = 30.0 + (speedLineAmount * 30);
    float frameRate = frames * fract(velocity);
    vec2 xy = IN.texCoord;

    if(speedLineDir ==1){
        xy.y-=1.0;
    } else if (speedLineDir == 2){
        xy.y+=1.0;
        frameRate *= 5.0;
    } else if(speedLineDir == 3){
        xy.x -=1.0;
    } else if(speedLineDir == 4){
        xy.x +=1.0;
    } else { xy = xy; }

    vec2 uvPolar = toPolar(xy, 0.5, 0.07, LINECOUNT);
    uvPolar = uRotate(uvPolar, frameRate, 0.5);


    vec3 col = vec3(0.);
    col += fbm(uvPolar * 30.0);

    float dist4center = dist(xy, vec2(0.5));

    float centerMaskSize = 0.06;
    float centerMaskEdge = 0.6;

    //figure these values out 
    if(speedLineDir != 0){
        centerMaskSize = 0.06;
        centerMaskEdge = 0.6;
    }

    float added = centerMaskEdge + centerMaskSize;

    vec3 mask = inverseLerp(centerMaskSize, added, vec3(dist4center));

    float lineDensity = 0.444;
    vec3 multipliedMask = mask *lineDensity;
    vec3 invmask = 1.0 - multipliedMask;

    float lineFalloff = 0.25;
    vec3 invFalloff = invmask + lineFalloff;

    vec3 fin = smoothstep(invmask, invFalloff, col);
    vec4 fincol = vec4(fin, smoothstep(0.2, 0.5, length(fin)));
    fincol *= speedLineAmount;
    
    if(SpeedLinesActive){
        fragColor += fincol;
    }
}
