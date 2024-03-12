#version 330

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform sampler2D hdrBuffer;
uniform sampler2D depthBuffer;
uniform mat4 invViewPersp;
uniform vec3 lightPos;

uniform float u_time;
uniform bool SpeedLinesActive;
uniform float speedLineAmount;
uniform int speedLineDir;

#define M_PI 3.1415926535897932384626433832795

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour[2];

void main() {

}