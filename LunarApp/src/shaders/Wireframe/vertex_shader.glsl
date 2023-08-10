#version 330 // version is 3.3.0

// input data to vertex shader (inside VAO)
layout (location = 0) in vec3 in_position; // X Y Z
layout (location = 1) in vec2 in_texture; // U V
layout (location = 2) in vec3 in_normal; // Nx Ny Nz

out vec2 TexCoord;
out vec3 Normal;

uniform mat4 Model; // Model position

void main(void)
{
    gl_Position = Model * vec4(in_position, 1.0f);
    TexCoord = in_texture;
    Normal = normalize(mat3(transpose(inverse(Model))) * in_normal);
}