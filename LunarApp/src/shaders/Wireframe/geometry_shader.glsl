#version 330

//https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
layout (triangles) in;
layout (line_strip, max_vertices=3) out;

in vec2 TexCoord[]; // texcoords from Vertex Shader
in vec3 Normal[]; // normals from Vertex Shader

out vec2 TexCoords; // texcoords for Fragment Shader
out vec3 Normals; // normals for Fragment Shader


void main(void)
{
    int i;
    for (i = 0; i < gl_in.length(); i++)
    {
        TexCoords = TexCoord[i]; // pass through
        Normals = Normal[i]; // pass through
        gl_Position = gl_in[i].gl_Position; // pass through
        EmitVertex();
    }
    EndPrimitive();
}
