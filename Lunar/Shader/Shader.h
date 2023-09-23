//
// Created by Minkyeu Kim on 7/17/23.
//

#ifndef SCOOP_SHADER_H
#define SCOOP_SHADER_H

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

	class Shader
	{
	private:
		std::string m_DebugName; // for debug purpose
		char	m_ShadersBitmap = 0; // 최소 요건 쉐이더 2개(F, V)가 세팅이 되어 있는지 체크.
		// [ 0 0 0 0 0 0 F V ] 8bit --> F=fragment_shader, V=vertex_shader
		GLuint	m_ProgramID = 0;
		GLint	m_UniformProjectionLocation = 0;
		GLint	m_UniformViewLocation = 0;
		GLint	m_UniformModelLocation = 0;

		typedef struct s_UniformDirectionLight {
			GLint 	DirectionLocation = 0;
			GLint	AmbientIntensityLocation = 0;
			GLint	DiffuseIntensityLocation = 0;
			GLint   SpecularIntensityLocation = 0;
		}	t_UniformDirectionLight;
		t_UniformDirectionLight m_UniformDirectionLight;

		typedef struct s_UniformMaterial {
			GLint	SpecularExponentLocation = 0;
			GLint	SpecularColorLocation = 0;
			GLint   AmbientColorLocation = 0;
			GLint 	DiffuseColorLocation = 0;
			GLint 	IndexOfRefractionLocation = 0;
			GLint 	DissolveLocation = 0;
			GLint 	IlluminationModelLocation = 0;
		} 	t_UniformMaterial;
		t_UniformMaterial m_UniformMaterial;

		GLint	m_UniformEyePosLocation = 0; // for Specular lighting
		GLint 	m_UniformShaderModeLocation = 0;

	public:
		explicit Shader(const std::string& name);
		~Shader();
		Shader& operator=(const Shader& other) = delete; // NOTE: forbid copy assign operator
		int AttachShader(const std::string& shaderPath, GLenum shaderType); // compile and attach shader to shaderProgram
		int LinkToGPU(); // link shaderProgram to GPU + load uniform variable location
		void DeleteFromGPU(); // delete program.
		Shader(const std::string& name,
					  const std::string& vertex_shader_path,
					  const std::string& fragment_shader_path,
					  const std::string& geometry_shader_path = ""
					  ); // attach 후 GPU에 link하는 과정 포함한 생성자.

		void Bind(); // bind shader program to GPU
		void Unbind(); // unbind shader from GPU

	public: // setter
		void SetUniformModel(const GLfloat *value) const;
		void SetUniformView(const GLfloat* value) const;
		void SetUniformProjection(const GLfloat *value) const;
		void SetUniformEyePos(const glm::vec3& eyePos) const;

	public: // getter
		inline GLuint GetProgramID() const { return m_ProgramID; }
		inline GLuint GetUniformShaderModeLocation() const { return m_UniformShaderModeLocation; }
		inline GLint GetUniformProjectionLocation() const { return m_UniformProjectionLocation; };
		inline GLint GetUniformModelLocation() const { return m_UniformModelLocation; };
		inline GLint GetUniformViewLocation() const { return m_UniformModelLocation; };

		inline t_UniformDirectionLight GetUniformDirectionLightLocations() const { return m_UniformDirectionLight; };
		inline t_UniformMaterial GetUniformMaterialLocations() const { return m_UniformMaterial; };

		inline std::string GetName() const { return m_DebugName; };

		// 각 쉐이더 헤더에서 업데이트 함수 구현.

	protected:
		static std::string _ReadFileToString(const std::string& path);
		GLint _GetUniformLocation(const char* name) const;

		virtual void OnBind() {};
		virtual void OnUnbind() {};
	};
}


#endif//SCOOP_SHADER_H
