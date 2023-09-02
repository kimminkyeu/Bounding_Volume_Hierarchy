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
    //    vec3 Color;
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

// A sampler cannot be "empty".
// A valid texture must be bound to the texture units referenced by
// each sampler in order for rendering to have well-defined behavior.

    // TODO: TextureUnit은 순수 이미지들의 로드 정보이고,
    // 각 material 에서는 위 Unit에 해당하는 숫자를 가져감으로서
    // 동일 이미지를 텍스쳐로 사용하는 경우 오버헤드 줄이기.

uniform sampler2D TextureUnit0; // Texture Unit
uniform uint ShaderMode; // use texture if 1
uniform s_DirectionLight DirectionLight;
uniform s_Material Material;
uniform vec3 EyePos;

// GPU instruction Set의 calling convention은 재귀를 허용하지 않는구만.
// unlike C or Java a function in GLSL cannot be called recursively.

// https://relativity.net.au/gaming/glsl/Functions.html
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
    // K = dot(Normal, HitToLight) | // P = HitToLight - K
    // ReflectedDir = HitToLight - 2 * (HitToLight - K) = 2K - HitToLight
    // ReflectedDir = 2*Dot(Normal, HitToLight)*HitToLight - HitToLight;
    // SpecularVal = Max(Dot(ReflectedDir, -EyeToHitPoint), 0.0f)
    vec4 specularColor = vec4(0.0f);
    float diffuseFactor = max(dot(-Normal, normalize(DirectionLight.Direction)), 0.0f);
    // 미리 불필요한 계산을 제거.
    if (diffuseFactor > 0) // 만약 빛 방향과 normal이 서로 반대방향이라면 specular 키기. 같은 방향이면, 반대임으로 specular를 적용할 필요 없음.
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
    switch(int(ShaderMode)) // uint8_t
    {
        case 0: // Wireframe
            FragmentColor = (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
            break;
        case 1: // Shaded
            FragmentColor = (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
            break;
        case 2: // Phong
            FragmentColor = texture(TextureUnit0, TexCoord) * (CalculateAmbient() + CalculateDiffuse() + CalculateSpecular());
            break;
        case 3: // Flat
            // ...
            break;
        case 4: // Rainbow
            FragmentColor = vec4(clamp(ModelPointPos, 0.0f, 1.0f), 1.0f);
            break;
        default:
            FragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        // TODO: Implement all Rhino3D Shaders
        // http://docs.mcneel.com/rhino/5/help/en-us/options/view_displaymode_options.htm
        // TODO: Artistic, Pen, Ghosted, Wireframe...
    }
}
