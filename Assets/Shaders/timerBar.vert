#version 330 core
layout (location = 0) in vec4 vertex; // 2d coords, and 2d texcoords

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;
uniform vec2 sizeRel;
uniform vec2 sizeAbs;
uniform vec2 positionRel;
uniform vec2 positionAbs;

void main()
{
    vec2 newVertex;
    newVertex = vertex.xy * sizeAbs;
    newVertex = newVertex.xy + vertex.xy * sizeRel;
    newVertex.x = newVertex.x + positionRel.x + positionAbs.x;
    newVertex.y = newVertex.y + positionRel.y + positionAbs.y;
    gl_Position = projection * model * vec4(newVertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}