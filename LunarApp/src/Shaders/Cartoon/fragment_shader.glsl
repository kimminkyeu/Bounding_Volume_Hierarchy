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

//https://roystan.net/articles/toon-shader/
vec4 CalculateToonDiffuse()
{
    float theta = dot(-Normal, normalize(DirectionLight.Direction));
    // float factor = theta > 0 ? 1 : 0; --> 단편적인 Factor --> 흑백 명암 2분화 --> 이를 부드럽게 하기 위한 SmoothFactor 적용
    // NOTE: smoothstep performs Hermite interpolation between two values
    float SmoothFactor = smoothstep(0, 0.05f, theta);
    vec4 diffuseColor = vec4(Material.DiffuseColor, 1.0f) * DirectionLight.DiffuseIntensity * SmoothFactor;
    return diffuseColor;
}

//https://roystan.net/articles/toon-shader/
vec4 CalculateToonSpecular()
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
        float specularSmoothFactor = smoothstep(0.005f, 0.01f, specularPowFactor);
        specularColor = vec4(Material.SpecularColor, 1.0f) * DirectionLight.SpecularIntensity * specularSmoothFactor;
    }
    return specularColor;
}

// Rim lighting. 외곽으로 갈수록 값이 커지면 된다.
// https://roystan.net/articles/toon-shader/
vec4 RimDot()
{
    const float RimAmount = 0.74f;
    const float RimThreshold = 0.1f;
    vec4 RimColor = vec4(Material.DiffuseColor, 1.0f); // used Diffuse Color as RimColor
    vec3 eyeToPointDir = normalize(ModelPointPos - EyePos);
    float RimDot = 1 - dot(eyeToPointDir, -Normal); // edge로 갈수록 cosine 값이 작아지니, 1을 빼서 뒤집는다.
    float nDotL = dot(-Normal, normalize(DirectionLight.Direction)); // normal이 빛을 향할 수록 1에 가까워짐.
    float RimIntensity = RimDot * pow(nDotL, RimThreshold);
    RimIntensity = smoothstep(RimAmount - 0.01, RimAmount + 0.01, RimIntensity);
    return RimColor * RimIntensity;
}

void main()
{
//    FragmentColor = texture(TextureUnit0, TexCoord) * (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
        FragmentColor = (CalculateAmbient() + CalculateToonDiffuse() + CalculateToonSpecular() + RimDot());
}
