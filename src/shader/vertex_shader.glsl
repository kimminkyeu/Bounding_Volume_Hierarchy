#version 330
// version is 3.3.0

// Vertex Shader = handling input of vertices.
layout (location = 0) in vec3 pos; // input data of vertex shader
out vec4 vertex_color; // output data of vertex shader

uniform mat4 model; // used in main code.
uniform mat4 projection; // used in main code.

void main()
{
    gl_Position = projection * model * vec4(pos, 1.0f);
    vertex_color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f); // because of -1.0f position in triangle...
}