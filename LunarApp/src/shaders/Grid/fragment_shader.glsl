#version 330 core // version is 3.3.0

out vec4 FragmentColor; // output data

uniform vec4 GridColor;

void main()
{
    FragmentColor = GridColor;
}
