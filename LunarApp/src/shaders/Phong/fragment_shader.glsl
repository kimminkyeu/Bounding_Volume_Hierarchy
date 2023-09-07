#version 330 core // version is 3.3.0

in vec2 TexCoord; // U V Coord
in vec3 Normal; // Normal
in vec3 ModelPointPos; // interpolated Fragment Position
out vec4 FragmentColor; // output data

// MESH PICKING TEST
// ****************************************************
uniform int PickMode; // if 0, diable picking
uniform vec3 PickedMeshV0; // picked triangle mesh v0
uniform vec3 PickedMeshV1; // picked triangle mesh v1
uniform vec3 PickedMeshV2; // picked triangle mesh v2
// ****************************************************

// 또 다른 방식은....
// size_t mesh id... --> 모든 삼각형 mesh (vbo)에 meshId를 넣어주고, 이를 그대로 읽어오는 방식...

// NOTE: 어떤 방식이 더 나은 방식일까? 잘 모르겠다...
// model point pos가 만약 위 v0 v1 v2 사이에 있는 값이라면, true 반환.
bool isInsidePickedMesh()
{
    // 주어진 점 3개 안에 model_point_pos가 있는지 체크.
//    vec3 cen = (PickedMeshV0 + PickedMeshV1 + PickedMeshV2) * 0.003f;
//    if ( ModelPointPos.x > cen.x ) return true;

    vec3 n0 = cross(ModelPointPos - PickedMeshV2, PickedMeshV1 - PickedMeshV2);
    if (dot(n0, Normal) < 0.0f) return false;
    vec3 n1 = cross(ModelPointPos - PickedMeshV0, PickedMeshV2 - PickedMeshV0);
    if (dot(n1, Normal) < 0.0f) return false;
    vec3 n2 = cross(PickedMeshV1 - PickedMeshV0, ModelPointPos - PickedMeshV0);
    if (dot(n2, Normal) < 0.0f) return false;
    return true;
}


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
    if (PickMode > 0 && isInsidePickedMesh())
    {
        FragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        // FragmentColor = texture(TextureUnit0, TexCoord) * (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
        FragmentColor = (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
    }
}
