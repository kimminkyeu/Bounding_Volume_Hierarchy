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
		FrameBuffer(float width, float height);
		~FrameBuffer();
		unsigned int GetFrameTexture() const;
		void RescaleFrameBuffer(float new_width, float new_height) const;
		void Bind() const;
		void Unbind() const;
		void Init(float width, float height);

	private:
		unsigned int m_FBO; // frame buffer object
		unsigned int m_RBO; // render buffer object
		unsigned int m_Texture; // texture object
	};
}

#endif//SCOOP_FRAMEBUFFER_H
