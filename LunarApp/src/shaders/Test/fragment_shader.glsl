#version 330 core // version is 3.3.0

in vec2 TexCoord; // U V Coord
in vec3 Normal; // Normal
in vec3 ModelPointPos; // interpolated Fragment Position
out vec4 FragmentColor; // output data

// mtl포맷을 보아 Intensity 값은 Material이 아닌 Light에 종속시키는게 맞다고 판단함.
struct s_DirectionLight {
    vec3 Direction;
    float AmbientIntensity;
    float DiffuseIntensity;
    float SpecularIntensity;
};

struct s_Material {
    float   SpecularExponent;   // Ns
    vec3    SpecularColor;      // Ks
    vec3    AmbientColor;       // Ka
    vec3    DiffuseColor;       // Kd
    float   IndexOfRefraction;  // Ni
    float   Dissolve;           // d (1-Tr)
    float   IlluminationModel;  // illum
};

uniform sampler2D TextureUnit0; // Texture Unit
uniform s_DirectionLight DirectionLight;
uniform s_Material Material;
uniform vec3 EyePos;

void main()
{
    FragmentColor = vec4(clamp(ModelPointPos, 0.0f, 1.0f), 1.0f);
}
