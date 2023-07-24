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
		: m_ProgramID(0), m_uniformModelLocation(-1), m_uniformProjectionLocation(-1), m_uniformViewLocation(-1), m_ShadersBitmap(0)
	{}

	ShaderProgram::ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
		: ShaderProgram::ShaderProgram()
	{
		// 1. Load shader
		LOG_TRACE("ShaderProgram constructor called");
		std::string vShaderPath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(vertex_shader_path);
		LOG_TRACE("vertex shader path: {0}", vShaderPath);
		this->attachShader(vShaderPath, GL_VERTEX_SHADER);
		std::string fShaderPath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(fragment_shader_path);
		LOG_TRACE("fragment shader path: {0}", fShaderPath);
		this->attachShader(fShaderPath, GL_FRAGMENT_SHADER);
		// 2. compile shader
		this->linkToGPU();
	}

	ShaderProgram::~ShaderProgram()
	{
		LOG_TRACE("ShaderProgram destructor called");
		this->deleteFromGPU();
	}

	std::string ShaderProgram::_readFileToString(const std::string& path)
	{
		std::ifstream t(path);
		if (t.fail()) {
			return {""};
		}
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}

	int ShaderProgram::attachShader(const std::string& shaderPath, GLenum shaderType)
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
		std::string shader_code = this->_readFileToString(shaderPath);
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
	int ShaderProgram::linkToGPU()
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

//		 find uniform variable inside the program. then, save its ID to member variable m_*.
		m_uniformModelLocation = glGetUniformLocation(m_ProgramID, "model");
		assert((m_uniformModelLocation >= 0) && "uniform model not found in shader");
		LOG_TRACE("GPU --> Uniform Model found in location {0}", m_uniformModelLocation);
//		 find uniform variable inside the program. then, save its ID to member variable m_*.
		m_uniformViewLocation = glGetUniformLocation(m_ProgramID, "view");
		assert((m_uniformViewLocation >= 0) && "uniform view not found in shader");
		LOG_TRACE("GPU --> Uniform View found in location {0}", m_uniformViewLocation);
//		 find uniform variable inside the program. then, save its ID to member variable m_*.
		m_uniformProjectionLocation = glGetUniformLocation(m_ProgramID, "projection");
		assert((m_uniformProjectionLocation >= 0) && "uniform projection not found in shader");
		LOG_TRACE("GPU --> Uniform Projection found in location {0}", m_uniformProjectionLocation);

		return (0);
	}

	void ShaderProgram::setUniformModel(const GLfloat *value) const
	{
		glUniformMatrix4fv(m_uniformModelLocation, 1, GL_FALSE, value); // GPU 변수 location에 값 대입.
	}

	void ShaderProgram::setUniformProjection(const GLfloat *value) const
	{
		glUniformMatrix4fv(m_uniformProjectionLocation, 1, GL_FALSE, value); // GPU 변수 location에 값 대입.
	}

	void ShaderProgram::setUniformView(const GLfloat *value) const
	{
		glUniformMatrix4fv(m_uniformViewLocation, 1, GL_FALSE, value); // GPU 변수 location에 값 대입.
	}

	GLint ShaderProgram::getUniformModelLocation() const
	{ return m_uniformModelLocation; }

	GLint ShaderProgram::getUniformProjectionLocation() const
	{ return m_uniformProjectionLocation; }

	GLint ShaderProgram::getUniformViewLocation() const
	{ return m_uniformViewLocation; }

	GLuint ShaderProgram::getProgramID() const
	{ return this->m_ProgramID; }

	void ShaderProgram::deleteFromGPU()
	{
		// delete program from GPU memory
		if (m_ProgramID)
		{
			glDeleteProgram(m_ProgramID);
			m_ProgramID = 0;
		}
		m_uniformModelLocation = 0;
		m_uniformProjectionLocation = 0;
		m_uniformViewLocation = 0;
		m_ShadersBitmap = 0;
	}
}