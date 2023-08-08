#version 330 core

// https://open.gl/geometry
// https://heinleinsgame.tistory.com/34
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

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

/*  geometry shader input varying variable must be declared as an array. The input to the Geometry shader is a primitives.
    That means all the outputs of the vertex shader which form a primitive are composed.
    Thus the inputs of the geometry shader are arrays:

    in vec3 some_data_passed_from_vertex_shader[];
*/

uniform float ExplosionDegree;

/* because gl_Postion is a Projected position, we can't use vertex shader's Normal variable */
vec3 GetNormalAfterProjection()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 Explode(vec4 position, vec3 normal)
{
    return (position + (vec4(normal, 0.0) * (ExplosionDegree)));
}

/*  The geometry shader program can call two special functions to generate primitives, EmitVertex and EndPrimitive.
    Each time the program calls EmitVertex, a vertex is added to the current primitive.
    When all vertices have been added, the program calls EndPrimitive to generate the primitive. */

void main()
{
    for (int i=0; i<gl_in.length(); ++i)
    {
        gl_Position = Explode(gl_in[i].gl_Position, GetNormalAfterProjection());
        EmitVertex();
    }
    EndPrimitive();
}
