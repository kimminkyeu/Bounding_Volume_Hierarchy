#version 330 // version is 3.3.0

layout(triangles) in; // input data is 3 vertices
layout(line_strip, max_vertices = 6) out; // Three lines will be generated: 6 vertices

uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)
// https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
// https://stackoverflow.com/questions/4703432/why-does-my-opengl-phong-shader-behave-like-a-flat-shader


in VS_OUT {
    vec3 Normal; // 여기서 Normal은 Model * View까지 계산한 결과임.
} gs_in[];

out vec3 Normal; // pass through fragment shader
const float MAGNITURE = 0.4f;

void main()
{
    // One line for the face normal
    // https://www.geeks3d.com/20130905/exploring-glsl-normal-visualizer-with-geometry-shaders-shader-library/
//    vec3 P0 = gl_in[0].gl_Position.xyz;
//    vec3 P1 = gl_in[1].gl_Position.xyz;
//    vec3 P2 = gl_in[2].gl_Position.xyz;

    // Center of the triangle
//    vec3 P = (P0+P1+P2) / 3.0;

// # Tree lines per triangle.
// ---------------------------------
    for (int i = 0; i < 3; i++)
    {
        gl_Position = Projection * gl_in[i].gl_Position; // Normal line start
        EmitVertex();
        gl_Position = Projection * (gl_in[i].gl_Position + vec4(gs_in[i].Normal * MAGNITURE, 0.0f)); // Normal line end
        EmitVertex();
        EndPrimitive();
    }
}