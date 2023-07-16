#version 330
// version is 3.3.0

in vec4 vCol; // input data (from vertex shader)
out vec4 color; // output data

// Fragment shader interpolates each vertex colors.
void main()
{
    color = vCol;
}
