//
// Created by Minkyeu Kim on 7/25/23.
//

#ifndef SCOOP_TEXTURE_H
#define SCOOP_TEXTURE_H

#include <string>
#include <GL/glew.h>
#include "Lunar/Core/Log.h"

namespace Lunar {

	// BitDepth : 색심도. 즉 한 비트가 표현할 수 있는 컬러 색상의 깊이값. (최대 256)
	// 참고: https://ko.wikipedia.org/wiki/%EC%83%89_%EA%B9%8A%EC%9D%B4

	class Texture
	{
	private:
		GLuint m_TextureID;
		int m_Width, m_Height, m_Channels;
		std::string m_FileLocation;

	public:
		Texture();
		Texture(const std::string& fileLocation);
		~Texture();
		bool LoadTexture();
		void UseTexture();
		void ClearTexture();

	};
}



#endif //SCOOP_TEXTURE_H
