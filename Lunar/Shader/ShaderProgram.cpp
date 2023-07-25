//
// Created by Minkyeu Kim on 7/17/23.
//

#include "ShaderProgram.h"
#include "Lunar/Core/Log.h"

// vertex_shader = 0000^0001
// fragment_shader = 0000^0010
static const char ESSENTIAL_SHADERS_LOADED = (1 << 1) | (1 << 0);

namespace Lunar {

	ShaderProgram::ShaderProgram()
		: m_ProgramID(0), m_UniformModelLocation(-1), m_UniformProjectionLocation(-1), m_UniformViewLocation(-1), m_ShadersBitmap(0)
	{}

	ShaderProgram::ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
		: ShaderProgram::ShaderProgram()
	{
		// 1. Load shader
		LOG_TRACE("ShaderProgram constructor called");
		std::string vShaderPath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(vertex_shader_path);
		LOG_TRACE("vertex shader path: {0}", vShaderPath);
		this->AttachShader(vShaderPath, GL_VERTEX_SHADER);
		std::string fShaderPath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(fragment_shader_path);
		LOG_TRACE("fragment shader path: {0}", fShaderPath);
		this->AttachShader(fShaderPath, GL_FRAGMENT_SHADER);
		// 2. compile shader
		this->LinkToGPU();
	}

	ShaderProgram::~ShaderProgram()
	{
		LOG_TRACE("ShaderProgram destructor called");
		this->DeleteFromGPU();
	}

	std::string ShaderProgram::_ReadFileToString(const std::string& path)
	{
		std::ifstream t(path);
		if (t.fail()) {
			return {""};
		}
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}

	int ShaderProgram::AttachShader(const std::string& shaderPath, GLenum shaderType)
	{
		if (!m_ProgramID) // if no program exist, then create program first.
		{
			m_ProgramID = glCreateProgram();
			if (!m_ProgramID)
			{
				LOG_ERROR("Error creating program");
				return (-2);
			}
		}
		// compile shader
		GLuint theShader = glCreateShader(shaderType);
		std::string shader_code = this->_ReadFileToString(shaderPath);
		assert(!shader_code.empty() && "Shader file-read Error");
		const GLint len = shader_code.length();
		const char* src = shader_code.c_str();
		glShaderSource(theShader, 1, &src, &len);
		glCompileShader(theShader);

		// check compile status
		GLint result = 0;
		glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			GLint maxLength = 0;
			glGetShaderiv(theShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(theShader, maxLength, nullptr, &infoLog[0]);
			glDeleteShader(theShader);
			LOG_ERROR("{0}", infoLog.data());
			return (-1);
		}
		// Need Check for this...?
		glAttachShader(this->m_ProgramID, theShader);

		// if all done, update loaded shader history.
		if (shaderType == GL_VERTEX_SHADER)
		{
			m_ShadersBitmap |= (1 << 0); // 0000^0001
		}
		else if (shaderType == GL_FRAGMENT_SHADER)
		{
			m_ShadersBitmap |= (1 << 1); // 0000^0010
		}

		return (0);
	}

	// 나중에 glCreateProgram 이 코드 다르게 만들기.
	int ShaderProgram::LinkToGPU()
	{
		GLint result = 0;
		if (m_ShadersBitmap != ESSENTIAL_SHADERS_LOADED)
		{
			assert(false && "Essential shaders (vertex, fragment) are not loaded");
		}

		// Link Program
		glLinkProgram(this->m_ProgramID);
		glGetProgramiv(this->m_ProgramID, GL_LINK_STATUS, &result);
		if (!result)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_ProgramID, maxLength, &maxLength, &infoLog[0]);
			glDeleteProgram(m_ProgramID);
			LOG_ERROR("LINK ERROR {0}", infoLog.data());
		}

		// Validate Program
		glValidateProgram(this->m_ProgramID);
		glGetProgramiv(this->m_ProgramID, GL_VALIDATE_STATUS, &result);
		if (!result)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_ProgramID, maxLength, &maxLength, &infoLog[0]);
			glDeleteProgram(m_ProgramID);
			LOG_ERROR("VALIDATION ERROR {0}", infoLog.data());
		}

		m_UniformModelLocation = glGetUniformLocation(m_ProgramID, "model");
		assert((m_UniformModelLocation >= 0) && "uniform model not found in shader");
		LOG_TRACE("GPU --> Uniform Model found in location {0}", m_UniformModelLocation);

		m_UniformViewLocation = glGetUniformLocation(m_ProgramID, "view");
		assert((m_UniformViewLocation >= 0) && "uniform view not found in shader");

		LOG_TRACE("GPU --> Uniform View found in location {0}", m_UniformViewLocation);
		m_UniformProjectionLocation = glGetUniformLocation(m_ProgramID, "projection");
		assert((m_UniformProjectionLocation >= 0) && "uniform projection not found in shader");
		LOG_TRACE("GPU --> Uniform Projection found in location {0}", m_UniformProjectionLocation);

		m_UniformAmbientColorLocation = glGetUniformLocation(m_ProgramID, "directionalLight.color");
		assert((m_UniformAmbientColorLocation >= 0) && "uniform directionalLight.color not found in shader");
		LOG_TRACE("GPU --> Uniform directionalLight found in location {0}", m_UniformAmbientColorLocation);

		m_UniformAmbientIntensityLocation = glGetUniformLocation(m_ProgramID, "directionalLight.ambientIntensity");
		assert((m_UniformAmbientIntensityLocation >= 0) && "uniform directionalLight.ambientIntensity not found in shader");
		LOG_TRACE("GPU --> Uniform ambientIntensity found in location {0}", m_UniformAmbientIntensityLocation);

		return (0);
	}

	void ShaderProgram::SetUniformModel(const GLfloat *value) const
	{
		glUniformMatrix4fv(m_UniformModelLocation, 1, GL_FALSE, value); // GPU 변수 location에 값 대입.
	}

	void ShaderProgram::SetUniformProjection(const GLfloat *value) const
	{
		glUniformMatrix4fv(m_UniformProjectionLocation, 1, GL_FALSE, value); // GPU 변수 location에 값 대입.
	}

	void ShaderProgram::SetUniformView(const GLfloat *value) const
	{
		glUniformMatrix4fv(m_UniformViewLocation, 1, GL_FALSE, value); // GPU 변수 location에 값 대입.
	}

	GLint ShaderProgram::GetUniformModelLocation() const
	{ return m_UniformModelLocation; }

	GLint ShaderProgram::GetUniformProjectionLocation() const
	{ return m_UniformProjectionLocation; }

	GLint ShaderProgram::GetUniformViewLocation() const
	{ return m_UniformViewLocation; }

	GLuint ShaderProgram::GetProgramID() const
	{ return this->m_ProgramID; }

	void ShaderProgram::DeleteFromGPU()
	{
		// delete program from GPU memory
		if (m_ProgramID)
		{
			glDeleteProgram(m_ProgramID);
			m_ProgramID = 0;
		}
		m_UniformModelLocation = 0;
		m_UniformProjectionLocation = 0;
		m_UniformViewLocation = 0;
		m_ShadersBitmap = 0;
	}

	GLint ShaderProgram::GetUniformAmbientColorLocation() const
	{ return m_UniformAmbientColorLocation; };

	GLint ShaderProgram::GetUniformAmbientIntensityLocation() const
	{ return m_UniformAmbientIntensityLocation; };
}
