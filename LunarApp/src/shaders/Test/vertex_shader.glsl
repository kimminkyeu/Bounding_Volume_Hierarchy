#version 330 // version is 3.3.0

// input data to vertex shader (inside VAO)
layout (location = 0) in vec3 in_position; // X Y Z
layout (location = 1) in vec2 in_texture; // U V
layout (location = 2) in vec3 in_normal; // Nx Ny Nz

out vec3 ModelPointPos; // model matrix만 곱한 오리지널 obj 좌표값.

uniform mat4 Model; // Model position
uniform mat4 View; // Camera View
uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)

void main(void)
{
    gl_Position = Projection * View * Model * vec4(in_position, 1.0f);
    ModelPointPos = (Model * vec4(in_position, 1.0f)).xyz; // change Vec4 to Vec3 of XYZ
}