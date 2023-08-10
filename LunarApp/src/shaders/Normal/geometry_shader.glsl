#version 330 // version is 3.3.0

layout(triangles) in; // input data is 3 vertices
layout(line_strip, max_vertices = 8) out; // 4 lines = 8 verticies

uniform mat4 Projection; // Camera Projection (perspective, orthogonal, etc...)
uniform mat4 View; // Camera Projection (perspective, orthogonal, etc...)

in VS_OUT {
    vec3 Normal;
} gs_in[];

out vec4 fragColor;
const float MAGNITURE = 0.4f;

void main()
{
    // (1) Tree lines per triangle
    for (int i = 0; i < 3; i++)
    {
        gl_Position = (Projection * View) * gl_in[i].gl_Position; // Normal line start
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        EmitVertex();

        gl_Position = (Projection * View) * (gl_in[i].gl_Position + vec4(gs_in[i].Normal * MAGNITURE, 0.0f)); // Normal line end
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        EmitVertex();

        EndPrimitive();
    }
    // (2) One line for the face normal
    vec3 P0 = gl_in[0].gl_Position.xyz; // v1
    vec3 P1 = gl_in[1].gl_Position.xyz; // v2
    vec3 P2 = gl_in[2].gl_Position.xyz; // v3
    vec3 P = (P0+P1+P2) / 3.0; // Center of the triangle
    vec3 CenterNormal = normalize(cross(P2 - P1, P0 - P1));
    gl_Position = Projection * View * vec4(P, 1.0f); // face normal start
    fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    EmitVertex();

    gl_Position = Projection * View * (vec4(P, 1.0f) + vec4(CenterNormal * MAGNITURE, 0.0f)); // Normal line end
    fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}