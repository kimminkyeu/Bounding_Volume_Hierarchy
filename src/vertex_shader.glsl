#version 330
// version is 3.3.0

// Vertex Shader = handling input of vertices.
layout (location = 0) in vec3 pos; // input data of vertex shader
out vec4 vCol; // output data of vertex shader

uniform mat4 matrix; // used in main code.

void main()
{
    gl_Position = matrix * vec4(pos, 1.0f);
    vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f); // because of -1.0f position in triangle...
}