#version 330 // version is 3.3.0

// input data to vertex shader (inside VAO)
layout (location = 0) in vec3 in_position; // X Y Z
layout (location = 1) in vec2 in_texture; // U V
layout (location = 2) in vec3 in_normal; // Nx Ny Nz

uniform mat4 Model; // Model position
//uniform mat4 View; // Camera View

out VS_OUT {
    vec3 Normal;
} vs_out;


void main(void)
{
    gl_Position = Model * vec4(in_position, 1.0f);
    // https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
    vs_out.Normal = normalize(mat3(transpose(inverse(Model))) * in_normal);
    // NOTE: 왜 View * Model을 넘기는지?
}