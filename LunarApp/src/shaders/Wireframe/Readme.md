
## WireFrame Display Mode를 구현하는 방법
1. glPolygonMode를 사용한다.
2. Geometry Shader로 외곽선을 칠해줄 면을 추가해준다.
3. Fragment Shader를 이용한다.

- https://stackoverflow.com/questions/137629/how-do-you-render-primitives-as-wireframes-in-opengl
Assuming a forward-compatible context in OpenGL 3 and up, you can either use glPolygonMode as mentioned before, but note that lines with thickness more than 1px are now deprecated. 
So while you can draw triangles as wire-frame, they need to be very thin. In OpenGL ES, you can use GL_LINES with the same limitation.
In OpenGL it is possible to use geometry shaders to take incoming triangles, disassemble them and send them for rasterization as quads (pairs of triangles really) emulating thick lines. 
Pretty simple, really, except that geometry shaders are notorious for poor performance scaling.
What you can do instead, and what will also work in OpenGL ES is to employ fragment shader. 
Think of applying a texture of wire-frame triangle to the triangle. Except that no texture is needed, it can be generated procedurally.