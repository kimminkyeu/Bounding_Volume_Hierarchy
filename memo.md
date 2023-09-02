
---
### 0724

1. window 크기 변경시 적절한 반영

   카메라, 뷰, 화면, 오브젝트 등 객체가 많아지다보니 각 객체간의 관계가 복잡해짐... 특히 , 화면비가 바뀌었을 때 카메라와 window 모두에게 glfwCallback을 전달해야 하는데.  이 부분 설계가 복잡해짐. 어떻게 해야 하지??
2. glfw 콜백 시스템 구성.
3. giszmo (x y z 회전 밑 이동 pivot) 구현
4. 쿼터니언 카메라 구현과 그 원리 이해 (수식) --> EditorCamera.h 참고

---
### 0725
1. scop 과제는 obj loading library를 사용할 수 없음.
   직접 데이터를 파싱해야만 함.

- 일단 obj 파일에 normal 데이터와 uv 데이터는 없다.
- 또한 f가 indices, 즉 한 면을 구성하는 점들을 의미한다.
- 그럼 내가 obj를 로드한 뒤에, 각 점의 normal과 (3점의 좌표를 아니까 가능하긴 함) uv를 자동으로 만들어주어야 하나?
- https://winterbloed.be/reading-an-obj-file-in-processing/

---
### 0727

##### ERROR
인터넷에서 다운받은 brick_wall.png가 자꾸 텍스쳐가 안되서..
(glTexImage2D에서 segfault 발생)...
그래서 이걸 강의제공 이미지인 brick.png로 바꿧더니 잘됨.
아니 시발 왜 이런거여
1. 후보 1. 강의 제공 이미지는 32bit color 데이터이고 내가 다운받은건 48bit.
이점 때문에 터진건지?
2. 후보 2. 그냥 내가 받은 brick_wall.png가 정상 이미지가 아닌 건지? 
   - 아니 뭐때매 터지는건지를 내가 알아야 이걸 예외처리를 할 거아냐...

##### PHONG
minirt 때 구현한 퐁 쉐이딩과는 조금 다르다.
- 그리고 수정해야 함~!
- 지금 normal 계산이 잘 못 됬는지 x방향 lighting이 먹지 않는다.
- 고침. normal 계산 잘못한거 맞음 ㅋㅋ


### OBJ loader 구현시 고려할 사항
```c++
const aiScene* scene = importer.ReadFile(fullTexturePath,
                          aiProcess_Triangulate | aiProcess_FlipUVs |
                          aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
```
여기 각 옵션 비트들의 의미를 분석해보면, 42과제에서 teapot1이 뭐가 잘못된 데이터가 들어있고, 이를 어떻게 고쳐야 하는지 설명한다.

### 텍스쳐가 없을 경우, 일반 Diffuse Color로 나오게끔 해야 함.

---
### 0728 문제점 노트
- 왜 texture가 없으면 shading이 먹지 않는 걸까?

### 0728 뭔가 깨달음 노트
Teselation이 추가된 이유가 바로 normal interpolation 정밀도 향상을 위한 것 아닐까.
예를 들어 폴리곤 수를 줄이기 위해 커다란 삼각형 면은 삼각형 한개로 처리한다고 하자.
이때, 각 삼각형 꼭지점의 normal을 gpu에서 자동 interpolation하다 보면
실제 빛에 면이 반사되는 것과 다르게 구처럼 빛이 맺힌다.
평면에 빛을 비추면, flat shading처럼 작동해야하는데 interpolation 때문에 의도와 다르게 되는 것이다.
tesellation의 목적이 여기에 있다고 생각된다..
큰 삼각형 polygon을 더 작게 쪼개주면, 그 쪼개진 면 내부 추가좌표들의 normal은 interpolating된 면이 아니다.
따라서 이 부분이 평면의 정확도를 높혀주는 것으로 보인다.