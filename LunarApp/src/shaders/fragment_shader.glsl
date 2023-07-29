#version 330 core
// version is 3.3.0

in vec2 TexCoord; // U V Coord
in vec3 VertexPos; // from vertex shader.
in vec3 Normal; // Normal

//in vec4 vColorTest; // just for test

// A sampler cannot be "empty".
// A valid texture must be bound to the texture units referenced by
// each sampler in order for rendering to have well-defined behavior.
uniform sampler2D TextureUnit0; // Texture Unit

// use texture if 1
uniform int hasTexture;

out vec4 fragmentColor; // output data

struct DirectionLight {
    vec3 color;
    vec3 direction;
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

uniform DirectionLight directionalLight;

// 원본 eye와 원본 point간의 계산...
// turn of texture
void main()
{
//    fragment_color = texture(TextureUnit0, TexCoord) * vColorTest;

    // Ambient Color
    vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

    // Diffuse Color
    float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
    vec4 diffuseColor = vec4(directionalLight.color, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

    // Specular Color;
    // TODO: add camera position to shader
    // K = dot(Normal, HitToLight)
    // P = HitToLight - K
    // ReflectedDir = HitToLight - 2 * (HitToLight - K) = 2K - HitToLight
    // ReflectedDir = 2*Dot(Normal, HitToLight)*HitToLight - HitToLight;
    // SpecularVal = Max(Dot(ReflectedDir, -EyeToHitPoint), 0.0f)
    vec3 lightDir = normalize(directionalLight.direction);
    vec3 lightReflectionDir = normalize(2 * dot(Normal, lightDir) * Normal - lightDir);
    // 아씨 생각해보면 fragShader 기준 카메라는 항상 중앙이네. (by view / transform matrix)
    vec3 eyeToPointDir = normalize(VertexPos);
    float specularFactor = max(dot(lightReflectionDir, -eyeToPointDir), 0.0f);
    float specularPow = pow(specularFactor, 20.0f);
    vec4 specularColor = vec4(directionalLight.color, 1.0f) * directionalLight.specularIntensity * specularPow;
    // camera position이 동적으로 바뀔텐데. 이 부분을 어떻게 해결하지?

    if (hasTexture == 1)
    {
        fragmentColor = texture(TextureUnit0, TexCoord) * (ambientColor + diffuseColor + specularColor);
    }
    else
    {
//        fragmentColor = (ambientColor + diffuseColor + specularColor);
//        fragmentColor = (ambientColor + diffuseColor);
        fragmentColor = (ambientColor + specularColor);
    }
}
