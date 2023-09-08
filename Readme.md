## Obj file viewer 
[Youtube Link](https://youtu.be/IXdCqPJfspo?si=NZ0x4q1wBHoGwQnw)

![제목 없는 동영상 - Clipchamp로 제작 (2)](https://github.com/kimminkyeu/MiniBlender/assets/60287070/df4a51de-530f-443a-bb34-0930e040f66d)
![image](https://github.com/kimminkyeu/MiniBlender/assets/60287070/eb0c17c9-8fd3-43de-923e-be2e1da90362)


## Compile & Run
this project is for Linux, MacOS. (Windows is currently not supported)
```bash
#1) compile with cmake (Realease version)
 cmake -DCMAKE_BUILD_TYPE=Release -B release && make -C release
#2) run program
 ./release/scoop
```



---
## To do List
1. faster BVH build (!!!)
2. Mesh picking (for texture coloring)
3. implement Global-illumination (ray-tracing)
4. store Mesh to Half-edge structure
5. optimize ray-tracing structure (threading 구조 변경 필요)
---

### 1. 참고 설계
0. SAH(surface area huristic) 을 이용한 Bounding Volume Hirachy 구현
0. Entity Component System (ECS) 방식으로 구조 작성할 것.
    - https://github.com/skypjack/entt#introduction
2. The Cherno GameEngine Series (Demo)
    - https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT
3. GameEngine Series Sandbox ```OpenGL```
    -  https://github.com/TheCherno/OpenGL/blob/master/OpenGL-Core/src/GLCore/Core/Application.cpp
4. Hazel Engine ```Vulkan```
    - https://github.com/TheCherno/Hazel
5. Walnut Framework ```Vulkan```
    - https://github.com/StudioCherno/Walnut/tree/20f940b9d23946d4836b8549ff3e2c0750c5d985/Walnut/src/Walnut
---
### 2. 목표
0. 프로젝트마다 해당 구조를 재사용 할 수 있도록 Template화 할 것.
1. ```OpenGL```로 먼저 구현 후, ```Vulkan```으로 이전.
2. 구조는 Hazel Engine을  분석하여 반영할 것.
3. ImGUI 로 초반 프로토타입만 작성한 후, ```QT GUI```로 최종 변환할 것.
4. Mac, Linux, Window 모두 지원되도록 ```CMake```로 build 환경 구성할 것.
    ```shell
   # 운영체제에 관계없이, git clone 후 아래 명령어만 실행하면 컴파일 되도록 설정할 것!
   cmake -B build && make -C build
   ```
---
