#version 330 // version is 3.3.0

// input data to vertex shader (inside VAO)
layout (location = 0) in vec3 in_position; // X Y Z

//uniform mat4 View; // Camera View
//uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)
uniform mat4 Model; // Model position

void main(void)
{
    gl_Position = Model * vec4(in_position, 1.0f);
//    gl_Position = Projection * View * Model * vec4(in_position, 1.0f);
}