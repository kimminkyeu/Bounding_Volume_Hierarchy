#version 330 core
// version is 3.3.0

in vec4 vertex_color; // input data (from vertex shader)
out vec4 fragment_color; // output data

// Fragment shader interpolates each vertex colors.
void main()
{
//    fragment_color = vertex_color;
    fragment_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
