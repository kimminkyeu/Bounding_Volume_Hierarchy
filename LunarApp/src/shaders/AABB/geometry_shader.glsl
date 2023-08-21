#version 330

//https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
layout (triangles) in;
layout (line_strip, max_vertices=4) out;

in vec2 TexCoord[]; // texcoords from Vertex Shader
in vec3 Normal[]; // normals from Vertex Shader

out vec2 TexCoords; // texcoords for Fragment Shader
out vec3 Normals; // normals for Fragment Shader

uniform mat4 View; // Camera View
uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)

const float OFFSET = 0.005f;
void main(void)
{
    int i;
    for (i = 0; i < gl_in.length()/*3*/; i++)
    {
        TexCoords = TexCoord[i]; // pass through
        Normals = Normal[i]; // pass through
        gl_Position = Projection * View * (gl_in[i].gl_Position + vec4(Normals * OFFSET, 0.0f)); // pass through
        EmitVertex();
    }
    // Loop을 위해 시작점 vertex를 하나 더 추가. (triangle_loop)
    TexCoords = TexCoord[0]; // pass through
    Normals = Normal[0]; // pass through
    gl_Position = Projection * View * (gl_in[0].gl_Position + vec4(Normals * OFFSET, 0.0f)); // pass through
    EmitVertex();

    EndPrimitive();
}
