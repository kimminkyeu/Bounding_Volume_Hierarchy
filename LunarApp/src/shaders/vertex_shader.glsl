#version 330
// version is 3.3.0

// input data to vertex shader (inside VAO)
layout (location = 0) in vec3 positionIn; // X Y Z
layout (location = 1) in vec2 textureIn; // U V
layout (location = 2) in vec3 normalIn; // Nx Ny Nz

out vec2 TexCoord;
out vec3 Normal;
out vec3 VertexPos;
//out vec4 vColorTest; // just for test

uniform mat4 model; // Model position
uniform mat4 view; // Camera View
uniform mat4 projection; // Camera Projection (perspective, orthogonal, etc...)

// Just like in C/C++, execution of a shader begins with the function main.
// This function takes no parameters and returns no values.
void main(void)
{
    gl_Position = projection * view * model * vec4(positionIn, 1.0f);
    VertexPos = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
    TexCoord = textureIn;
//    vColorTest = vec4(clamp(position_in, 0.0f, 1.0f), 1.0f);

    // model matrix에 의한 normal 변화 반영.
    // 이때, 모델에 대한 scale정보만 normal에 반영되기 때문에 (이동은 x), 굳이 4차원으로 가져갈 이유 없음.
    // *********************************************************
    // NOTE: WHat is Normal Transformation?
    // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
    // https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals.html
    // *********************************************************
    Normal = mat3(transpose(inverse(model))) * normalIn;

}