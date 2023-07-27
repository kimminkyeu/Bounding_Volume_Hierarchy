#version 330 core
// version is 3.3.0

in vec2 TexCoord; // U V Coord
in vec3 Normal; // Normal

//in vec4 vColorTest; // just for test

uniform sampler2D TextureUnit0; // Texture Unit

out vec4 fragment_color; // output data

struct DirectionLight {
    vec3 color;
    vec3 direction;
    float ambientIntensity;
    float diffuseIntensity;
};
uniform DirectionLight directionalLight;

void main()
{
//    fragment_color = texture(TextureUnit0, TexCoord) * vColorTest;
    vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

    // same as MiniRT diffuse calculation.
    float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
    vec4 diffuseColor = vec4(directionalLight.color, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

    fragment_color = texture(TextureUnit0, TexCoord) * (ambientColor + diffuseColor);
}
