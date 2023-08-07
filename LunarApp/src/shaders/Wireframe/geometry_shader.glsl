#version 330 core

// https://open.gl/geometry
layout(points) in;
layout(line_strip, max_vertices = 2) out;

/*  The gl_Position, as set in the vertex shader, can be accessed using the gl_in array in the geometry shader.
    It is an array of structs that looks like this:
    ------------------------------------------------
    in gl_PerVertex
    {
        vec4 gl_Position;
        float gl_PointSize;
        float gl_ClipDistance[];
    } gl_in[];
*/

/*  The geometry shader program can call two special functions to generate primitives, EmitVertex and EndPrimitive.
    Each time the program calls EmitVertex, a vertex is added to the current primitive.
    When all vertices have been added, the program calls EndPrimitive to generate the primitive. */
void main()
{
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}