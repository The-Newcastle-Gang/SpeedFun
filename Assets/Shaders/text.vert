#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform mat2 rotation;

void main()
{
    vec2 afterRotation = rotation * vertex.xy;
    gl_Position = projection * vec4(afterRotation, 0.0, 1.0);
    TexCoords = vertex.zw;
}  