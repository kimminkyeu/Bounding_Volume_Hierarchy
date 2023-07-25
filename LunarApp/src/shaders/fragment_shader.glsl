#version 330 core
// version is 3.3.0

in vec2 TexCoord; // U V Coord

uniform sampler2D TextureUnit0; // Texture Unit

uniform struct s_DirectionalLight
{
    vec3 color;
    float ambientIntensity;
};

out vec4 fragment_color; // output data

uniform s_DirectionalLight directionalLight;

// Fragment shader interpolates each vertex colors.
void main()
{
    vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;
    fragment_color = texture(TextureUnit0, TexCoord);
}
