//
// Created by USER on 2023-07-24.
//

#ifndef SCOOP_EDITORCAMERA_H
#define SCOOP_EDITORCAMERA_H

#include "CameraBase.h"

// Entity Component System
	// https://www.youtube.com/watch?v=Z-CILn2w9K0&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=76
	// https://www.youtube.com/watch?v=D4hz0wEB978

// Camera System
	// https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Renderer/EditorCamera.h
	// https://www.youtube.com/watch?v=lXlXqUEEJ94&t=655s
	// https://www.youtube.com/watch?v=q7rwMoZQSmw

// RayTracing in CPU, Camera System Hint
	// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Camera.h

// 어떻게 RT 구조를 짯는지 보니까... 소스 보니까 image를 화면에 찍는 방식으로 레이트레이싱을 했네.
// 걍 이미지 하나를 그리고, 그걸 ImGui::Image 에다가 때려 박았네...
	// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/WalnutApp.cpp#L120 --> 여기 참고 !

namespace Lunar {
	class EditorCamera : public CameraBase
	{
	public:

	private:

	};
}



#endif //SCOOP_EDITORCAMERA_H
