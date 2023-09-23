#version 330 // version is 3.3.0

// input data to vertex shader (inside VAO)
layout (location = 0) in vec3 in_position; // X Y Z
layout (location = 1) in vec2 in_texture; // U V
layout (location = 2) in vec3 in_normal; // Nx Ny Nz

uniform mat4 Model; // Model position
uniform mat4 View; // Camera View
uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)

// https://stackoverflow.com/questions/27098315/render-large-circular-points-in-modern-opengl
void main(void)
{
    const float OFFSET = 0.005f;
    vec3 Normal = normalize(mat3(transpose(inverse(Model))) * in_normal);
    gl_Position = Projection * View * Model * vec4(in_position + (Normal * OFFSET), 1.0f);
    gl_PointSize = 7.0f;
}