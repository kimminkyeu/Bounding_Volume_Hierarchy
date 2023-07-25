//
// Created by Minkyeu Kim on 7/25/23.
//

#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Lunar {

	Texture::Texture()
			: m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_FileLocation("")
	{}

	Texture::Texture(const std::string& fileLocation)
		: m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_FileLocation(fileLocation)
	{}

	Texture::~Texture()
	{

	}

	void Texture::UseTexture()
	{
		// Fragment shader에서 접근할 수 있는 texture unit 0. (쉽게 생각하면 내부 변수)
		glActiveTexture(GL_TEXTURE0); // use texture unit 0
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		// 이렇게 하면, frag_shader의 texture unit 0 를 접근하면 위 texture가 불러와지는 거임.
	}

	bool Texture::LoadTextureRGB()
	{
		std::string fullTexturePath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(m_FileLocation);
		unsigned char* textureData = stbi_load(fullTexturePath.c_str(), &m_Width, &m_Height, &m_Channels, 0);
		if (!textureData)
		{
			LOG_ERROR("Failed to find texture at {0}", fullTexturePath);
			return false;
		}
		LOG_TRACE("Loaded texture path: {0}", fullTexturePath);
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// GL_LINEAR : try to blend pixels
		// GL_NEAREST :
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// internal format = automatic mipMap generation (0)
		// PNG 이미지는 투명도가 존재하기 때문에, GL_RGBA로 format 지정함.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(textureData);
		return (true);
	}

	// if load success, return true
	bool Texture::LoadTextureRGBA()
	{
		std::string fullTexturePath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(m_FileLocation);
		unsigned char* textureData = stbi_load(fullTexturePath.c_str(), &m_Width, &m_Height, &m_Channels, 0);
		if (!textureData)
		{
			LOG_ERROR("Failed to find texture at {0}", fullTexturePath);
			return false;
		}
		LOG_TRACE("Loaded texture path: {0}", fullTexturePath);
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR : try to blend pixels // GL_NEAREST :
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// internal format = automatic mipMap generation (0) // PNG 이미지는 투명도가 존재하기 때문에, GL_RGBA로 format 지정함.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(textureData);
	}

	void Texture::ClearTexture()
	{
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
		m_Width = 0;
		m_Height = 0;
		m_Channels = 0;
		m_FileLocation.clear();
	}


}

