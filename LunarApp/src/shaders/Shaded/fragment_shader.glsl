#version 330 core // version is 3.3.0


out vec4 FragmentColor; // output data

in  VS_OUT {
    vec2 TexCoord; // texcoords from Vertex Shader
    flat vec3 Normal; // normals from Vertex Shader
    vec3 ModelPointPos; // model matrix만 곱한 오리지널 obj 좌표값.
}   fs_in;

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

vec4 CalculateAmbient()
{
    vec4 ambientColor = vec4(Material.AmbientColor, 1.0f) * DirectionLight.AmbientIntensity;
    return ambientColor;
}

vec4 CalculateDiffuse()
{
    float diffuseFactor = max(dot(-fs_in.Normal, normalize(DirectionLight.Direction)), 0.0f);
    vec4 diffuseColor = vec4(Material.DiffuseColor, 1.0f) * DirectionLight.DiffuseIntensity * diffuseFactor;
    return diffuseColor;
}

void main()
{
    FragmentColor = (CalculateAmbient() + CalculateDiffuse());
//    FragmentColor = (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
//    FragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
