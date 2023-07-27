#version 330
// version is 3.3.0

layout (location = 0) in vec3 position_in; // input data of vertex shader
layout (location = 1) in vec2 texture_in; // input data of vertex shader

out vec2 TexCoord;
out vec4 vColorTest;

uniform mat4 model; // Model position
uniform mat4 view; // Camera View
uniform mat4 projection; // Camera Projection (perspective, orthogonal, etc...)

// Just like in C/C++, execution of a shader begins with the function main.
// This function takes no parameters and returns no values.
void main(void)
{
    gl_Position = projection * view * model * vec4(position_in, 1.0f);
    TexCoord = texture_in;
    vColorTest = vec4(clamp(position_in, 0.0f, 1.0f), 1.0f);
}