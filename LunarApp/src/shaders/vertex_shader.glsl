#version 330 // version is 3.3.0

// input data to vertex shader (inside VAO)
layout (location = 0) in vec3 in_position; // X Y Z
layout (location = 1) in vec2 in_texture; // U V
layout (location = 2) in vec3 in_normal; // Nx Ny Nz

out vec2 TexCoord;
out vec3 Normal;
out vec3 ModelPointPos; // model matrix만 곱한 오리지널 obj 좌표값.

uniform mat4 Model; // Model position
uniform mat4 View; // Camera View
uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)

// Just like in C/C++, execution of a shader begins with the function main.
// This function takes no parameters and returns no values.
void main(void)
{
    gl_Position = Projection * View * Model * vec4(in_position, 1.0f);
    TexCoord = in_texture;

    // *********************************************************
    // NOTE: WHat is Normal Transformation?
    // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
    // https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals.html
    // *********************************************************
    Normal = normalize(mat3(transpose(inverse(Model))) * in_normal);

    // T.xy | T.xxy | t.zyx ... GLSL의 기능이다. 좌표를 원하는대로 추출 가능함.
    ModelPointPos = (Model * vec4(in_position, 1.0f)).xyz; // change Vec4 to Vec3 of XYZ
}