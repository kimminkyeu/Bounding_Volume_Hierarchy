#version 330 core // version is 3.3.0

in vec2 TexCoord; // U V Coord
in vec3 Normal; // Normal
in vec3 ModelPointPos; // interpolated Fragment Position
out vec4 FragmentColor; // output data

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
    float diffuseFactor = max(dot(-Normal, normalize(DirectionLight.Direction)), 0.0f);
    vec4 diffuseColor = vec4(Material.DiffuseColor, 1.0f) * DirectionLight.DiffuseIntensity * diffuseFactor;
    return diffuseColor;
}

vec4 CalculateSpecular()
{
    vec4 specularColor = vec4(0.0f);
    float diffuseFactor = max(dot(-Normal, normalize(DirectionLight.Direction)), 0.0f);
    if (diffuseFactor > 0)
    {
        vec3 lightDir = normalize(DirectionLight.Direction);
        vec3 lightReflectionDir = normalize(2 * dot(-Normal, lightDir) * Normal - lightDir);
        vec3 eyeToPointDir = normalize(ModelPointPos - EyePos);
        float specularFactor = max(dot(lightReflectionDir, -eyeToPointDir), 0.0f);
        float specularPowFactor = pow(specularFactor, Material.SpecularExponent);
        specularColor = vec4(Material.SpecularColor, 1.0f) * DirectionLight.SpecularIntensity * specularPowFactor;
    }
    return specularColor;
}

void main()
{
//    FragmentColor = texture(TextureUnit0, TexCoord) * (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
    FragmentColor = (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
//    FragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
