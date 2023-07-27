
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
인터넷에서 다운받은 brick_wall.png가 자꾸 텍스쳐가 안되서..
(glTexImage2D에서 segfault 발생)...
그래서 이걸 강의제공 이미지인 brick.png로 바꿧더니 잘됨.
아니 시발 왜 이런거여
1. 후보 1. 강의 제공 이미지는 32bit color 데이터이고 내가 다운받은건 48bit.
이점 때문에 터진건지?
2. 후보 2. 그냥 내가 받은 brick_wall.png가 정상 이미지가 아닌 건지? 

- 아니 뭐때매 터지는건지를 내가 알아야 이걸 예외처리를 할 거아냐...