#version 330 // version is 3.3.0

layout (location = 0) in vec3 in_position; // X Y Z
uniform mat4 Projection;
uniform mat4 View;

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[](
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, -1, 0), vec3(1, 1, 0)
);

void main(void)
{
    gl_Position = Projection * View * vec4(gridPlane[gl_VertexID].xyz, 1.0);
    gl_PointSize = 10.0f;
}