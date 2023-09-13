#version 330 // version is 3.3.0

layout (location = 0) in vec3 in_position; // X Y Z
uniform mat4 Projection;
uniform mat4 View;

out mat4 fragProj;
out mat4 fragView;
out vec3 nearPoint;
out vec3 farPoint;

// NOTE: Infinite Grid
// https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[](
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, -1, 0), vec3(1, 1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main(void)
{
    fragProj = Projection;
    fragView = View;
    // gl_VertexID = vertex count (6 draw call) --> from "glDrawArrays(GL_TRIANGLES, 0, 6)"
    vec3 p = gridPlane[gl_VertexID].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, View, Projection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, View, Projection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}