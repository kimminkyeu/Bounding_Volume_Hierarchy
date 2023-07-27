#version 330 core
// version is 3.3.0

in vec2 TexCoord; // U V Coord
in vec4 vColorTest; // just for test

uniform sampler2D TextureUnit0; // Texture Unit
out vec4 fragment_color; // output data

void main()
{
    fragment_color = texture(TextureUnit0, TexCoord) * vColorTest;
}
