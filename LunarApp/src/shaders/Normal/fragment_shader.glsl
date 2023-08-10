#version 330 // version is 3.3.0

in vec4 fragColor; // from geometry shader
out vec4 FragmentColor; // output data

void main()
{
//    FragmentColor = vec4(Normal, 1.0f);
    FragmentColor = fragColor;
}
