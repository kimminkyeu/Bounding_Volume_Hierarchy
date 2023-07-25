#version 330 core
// version is 3.3.0

in vec4 vertex_color;
in vec2 TexCoord; // U V Coord

out vec4 fragment_color; // output data

uniform sampler2D TextureUnit0; // Texture Unit

// Fragment shader interpolates each vertex colors.
void main()
{
    fragment_color = texture(TextureUnit0, TexCoord) * vertex_color;
}
