#version 330

//https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
layout (triangles) in;
layout (line_strip, max_vertices=3) out;

uniform mat4 View; // Camera View
uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)

const float OFFSET = 0.000f;

vec3 GetFaceNormal()
{
    vec3 a = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main(void)
{
    int i;
//    vec3 Normals = GetFaceNormal(); // pass through
    for (i = 0; i < gl_in.length()/*3*/; i++)
    {
//        gl_Position = Projection * View * (gl_in[i].gl_Position + vec4(Normals * OFFSET, 0.0f)); // pass through
        gl_Position = Projection * View * (gl_in[i].gl_Position); // pass through
        EmitVertex();
    }
    EndPrimitive();
}
