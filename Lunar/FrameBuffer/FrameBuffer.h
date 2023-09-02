//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_FRAMEBUFFER_H
#define SCOOP_FRAMEBUFFER_H

#include <GL/glew.h>

namespace Lunar {

	class FrameBuffer
	{
	public:
		FrameBuffer();
		FrameBuffer(uint32_t width, uint32_t height);
		~FrameBuffer();
		unsigned int GetFrameTexture() const;
		void Resize(uint32_t new_width, uint32_t new_height);
		void Bind() const;
		void Unbind() const;
		void Init(uint32_t width, uint32_t height);
		void LoadPixelsToTexture(const void* pixel_array);
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

	private:
		unsigned int m_FBO; // frame buffer object
		unsigned int m_RBO; // render buffer object
		unsigned int m_Texture; // texture object
		uint32_t m_Width = 0, m_Height = 0;
	};
}

#endif//SCOOP_FRAMEBUFFER_H
