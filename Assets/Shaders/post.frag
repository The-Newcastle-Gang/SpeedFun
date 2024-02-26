#version 330

out vec4 fragColor;
uniform sampler2D hdrBuffer;
uniform sampler2D depthBuffer;
uniform mat4 invViewPersp;
uniform vec3 lightPos;

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



void main() {
    vec2 uv = IN.texCoord.xy * 2.0 - 1.0;

    vec4 hdrColor = texture(hdrBuffer, IN.texCoord).rgba;
    vec3 rayOrigin = unproject(vec3(uv.x, uv.y, -1.0f));
    vec3 rayDirection = normalize(unproject(vec3(uv.x, uv.y, 1.0f)) - rayOrigin);
    vec4 raySphere = ray_march(rayOrigin, rayDirection);

    fragColor = vec4(raySphere.xyz + hdrColor.xyz * (1 - raySphere.a), raySphere.a + hdrColor.a);
}
