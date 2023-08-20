#version 330 core // version is 3.3.0

out vec4 FragmentColor; // output data
uniform vec4 PointColor;

void main()
{
    // https://stackoverflow.com/questions/27098315/render-large-circular-points-in-modern-opengl
    /*  gl_PointCoord is a fragment language input variable that contains the two-dimensional coordinates indicating where within a point primitive the current fragment is located.
        If the current primitive is not a point (GL_POINTS), then values read from gl_PointCoord are undefined. */
    // vertexShader에서 glPointSize를 10으로 정했다면, 10 * 10 pixel이 하나의 fragment 단위가 된다.
    // 즉 10*10 픽셀 기준으로, 해당 point가 pixel의 어느 부분인지 상대적 위치를 알려주는 값임.
    // ( gl_PointCoord가 vec2(0,0) 라면 10*10픽셀의 좌상단 )
    // ( gl_PointCoord가 vec2(1,1) 라면 10*10픽셀의 우하단 )
    // 아래 행위는 0~1 범위를 -1~1 범위로 바꾸는 행위임. 이를 통해 사각형을 원으로 깎는 과정이다.
    // 상대 그림은 오른쪽 링크 참조 // https://opengl-notes.readthedocs.io/en/latest/topics/particles-mapping/particles.html
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0; // NOTE: This is much useful in Particle System!!
    // discard everything outside a circle based on the distance from the center:
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    }
    FragmentColor = PointColor;
}
