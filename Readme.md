### Bug 
AABB 를 반으로 잘라서, 삼각형의 중심점 기준 위치에 따라 왼쪽, 오른쪽 Bounding Box 생성.
현재 AABB Debug Render에서 Box가 전부 안나오고 일부 잘리는 현상 발생.
이 이유가 Mesh를 생성할때인지, AABBTree를 만들때 발생하는 문제인지 체크해야 함.
![image](https://github.com/kimminkyeu/MiniBlender/assets/60287070/bef6eb0f-af04-4a2a-ba2e-f83f4d49b900)
