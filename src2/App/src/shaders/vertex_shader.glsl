#version 330
// version is 3.3.0

// Vertex Shader = handling input of vertices.
layout (location = 0) in vec3 pos; // input data of vertex shader
out vec4 vertex_color; // output data of vertex shader

//uniform mat4 model; // used in main code : Model position
//uniform mat4 view; // used in main code : Camera view
//uniform mat4 projection; // used in main code : Projection (perspective, orthogonal, etc...)

// Just like in C/C++, execution of a shader begins with the function main.
// This function takes no parameters and returns no values.
void main(void)
{
//    gl_Position = projection * view * model * vec4(pos, 1.0f);
    gl_Position = vec4(pos, 1.0f);
    vertex_color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f); // because of -1.0f position in triangle...
}