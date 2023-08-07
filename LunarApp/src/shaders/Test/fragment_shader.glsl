#version 330 core // version is 3.3.0

in vec3 ModelPointPos; // interpolated Fragment Position
out vec4 FragmentColor; // output data

void main()
{
    FragmentColor = vec4(clamp(ModelPointPos, 0.0f, 1.0f), 1.0f);
}
