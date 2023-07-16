#version 330
// version is 3.3.0

// Vertex Shader = handling input of vertices.
layout (location = 0) in vec3 pos;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0f);
}