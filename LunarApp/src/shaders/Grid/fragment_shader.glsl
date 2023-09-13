#version 330 core // version is 3.3.0

in vec3 nearPoint; // nearPoint calculated in vertex shader
in vec3 farPoint; // farPoint calculated int vertex shader
in mat4 fragView;
in mat4 fragProj;

out vec4 FragmentColor; // output data

uniform vec4 GridColor;
uniform float NearClip;
uniform float FarClip;

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * NearClip * FarClip) / (FarClip + NearClip - clip_space_depth * (FarClip - NearClip)); // get linear value between 0.01 and 100
    return linearDepth / FarClip; // normalize
}

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    float LINE_WIDTH = 0.2f;

    // z axis
    if ((fragPos3D.x > LINE_WIDTH * (-minimumx)) && (fragPos3D.x < LINE_WIDTH * minimumx))
    {
        return vec4(0.0, 0.0, 1.0, 1.0 - min(line, 1.0));
    }
    // x axis
    else if ((fragPos3D.z > LINE_WIDTH * (-minimumz)) && (fragPos3D.z < LINE_WIDTH * minimumz))
    {
        return vec4(1.0, 0.0, 0.0, 1.0 - min(line, 1.0));
    }
    else
    {
        float k = 1.0 - min(line, 1.0); // k <= 0 일 경우, 그리드 선 외의 부분.
        if (k > 0.5) { // line 두께 0.5
            vec4 color = vec4(GridColor.xyz, 1.0 - min(line, 1.0));
            return color;
        } else {
            discard;
        }
    }
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

void main()
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    // https://stackoverflow.com/questions/72791713/issues-with-infinite-grid-in-opengl-4-5-with-glsl
    gl_FragDepth = ((gl_DepthRange.diff * computeDepth(fragPos3D)) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max((1.0f - linearDepth), 0.0f); // 0.0 ~ 1.0f
    float fadingPow = pow(fading, 8.0f);

    if (fading < 0.2f) {
        FragmentColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
//        FragmentColor = grid(fragPos3D, 1, true) * float(t > 0);
        FragmentColor = (grid(fragPos3D, 10, true) + grid(fragPos3D, 1, true)) * float(t > 0); // adding multiple resolution for the grid
        FragmentColor *= fadingPow;
    }
}
