//
// Created by Minkyeu Kim on 7/17/23.
//

#ifndef SCOOP_SHADERPROGRAM_H
#define SCOOP_SHADERPROGRAM_H

#include <string>
#include <iostream>
#include <fstream> // ifstream
#include <sstream> // stringstream
#include <cassert> // assert
#include <cstring> // memset
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lunar/Config.h" // project configuration

namespace Lunar {

	class ShaderProgram
	{
	private:

		GLuint	m_ProgramID;
		GLint	m_UniformProjectionLocation;
		GLint	m_UniformViewLocation;
		GLint	m_UniformModelLocation;
		char	m_ShadersBitmap; // 최소 요건 쉐이더 2개(F, V)가 세팅이 되어 있는지 체크.
		//               F V    ( F=fragment_shader, V=vertex_shader )
		// [ 0 0 0 0 0 0 0 0 ] 8bit

		GLint	m_UniformAmbientIntensityLocation;
		GLint	m_UniformAmbientColorLocation;

	public:
		ShaderProgram();
//		ShaderProgram(ShaderProgram&& proc) noexcept;
		~ShaderProgram();
		// compile and attach shader to shaderProgram
		int AttachShader(const std::string& shaderPath, GLenum shaderType);
		// link shaderProgram to GPU + load uniform variable location
		int LinkToGPU();
		// attach 후 GPU에 link하는 과정 포함한 생성자.
		ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

		void SetUniformModel(const GLfloat *value) const;
		void SetUniformView(const GLfloat* value) const;
		void SetUniformProjection(const GLfloat *value) const;


		// delete program.
		void DeleteFromGPU();

		// get location of uniform variables inside GPU...
		GLuint GetProgramID() const;
		GLint GetUniformProjectionLocation() const;
		GLint GetUniformModelLocation() const;
		GLint GetUniformViewLocation() const;
		GLint GetUniformAmbientColorLocation() const;
		GLint GetUniformAmbientIntensityLocation() const;

	private:
		static std::string _ReadFileToString(const std::string& path);
	};
}


#endif //SCOOP_SHADERPROGRAM_H
