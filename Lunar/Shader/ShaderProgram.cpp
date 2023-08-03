//
// Created by Minkyeu Kim on 7/17/23.
//

#include "ShaderProgram.h"
#include "Lunar/Core/Log.h"

// vertex_shader = 0000^0001
// fragment_shader = 0000^0010
static const char ESSENTIAL_SHADERS_LOADED = (1 << 1) | (1 << 0);

namespace Lunar {

	ShaderProgram::ShaderProgram(const std::string& name)
		: m_DebugName(name), m_ProgramID(0), m_UniformModelLocation(-1), m_UniformProjectionLocation(-1), m_UniformViewLocation(-1), m_ShadersBitmap(0)
	{}

	ShaderProgram::ShaderProgram(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path)
		: ShaderProgram::ShaderProgram(name)
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

		// *************************** Model View Projection ************************
		m_UniformModelLocation = _GetUniformLocation("Model");
		m_UniformViewLocation = _GetUniformLocation("View");
		m_UniformProjectionLocation = _GetUniformLocation("Projection");
		// ***************************** For Phong Shading ******************************
		m_UniformDirectionLight.DirectionLocation = _GetUniformLocation("DirectionLight.Direction");
		m_UniformDirectionLight.AmbientIntensityLocation = _GetUniformLocation("DirectionLight.AmbientIntensity");
		m_UniformDirectionLight.DiffuseIntensityLocation = _GetUniformLocation("DirectionLight.DiffuseIntensity");
		m_UniformDirectionLight.SpecularIntensityLocation = _GetUniformLocation("DirectionLight.SpecularIntensity");
		m_UniformEyePosLocation = _GetUniformLocation("EyePos");
		m_UniformMaterial.SpecularExponentLocation = _GetUniformLocation("Material.SpecularExponent");
		m_UniformMaterial.SpecularColorLocation = _GetUniformLocation("Material.SpecularColor");
		m_UniformMaterial.AmbientColorLocation = _GetUniformLocation("Material.AmbientColor");
		m_UniformMaterial.DiffuseColorLocation = _GetUniformLocation("Material.DiffuseColor");
		m_UniformMaterial.IndexOfRefractionLocation = _GetUniformLocation("Material.IndexOfRefraction");
		m_UniformMaterial.DissolveLocation = _GetUniformLocation("Material.Dissolve");
		m_UniformMaterial.IlluminationModelLocation = _GetUniformLocation("Material.IlluminationModel");

		return (0);
	}

	void ShaderProgram::SetUniformModel(const GLfloat *value) const
	{ glUniformMatrix4fv(m_UniformModelLocation, 1, GL_FALSE, value); }

	void ShaderProgram::SetUniformProjection(const GLfloat *value) const
	{ glUniformMatrix4fv(m_UniformProjectionLocation, 1, GL_FALSE, value); }

	void ShaderProgram::SetUniformView(const GLfloat *value) const
	{ glUniformMatrix4fv(m_UniformViewLocation, 1, GL_FALSE, value); }

	void ShaderProgram::SetUniformEyePos(const glm::vec3& eyePos) const
	{ glUniform3f(m_UniformEyePosLocation, eyePos.x, eyePos.y, eyePos.z); }

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

	GLint ShaderProgram::_GetUniformLocation(const char* uniformName) const
	{
		GLint location = glGetUniformLocation(m_ProgramID, uniformName);
		if (location < 0)
		{
			LOG_WARN("  [{0} shader]: Uniform [{1}] not found.", m_DebugName, uniformName);
		}
		return location;
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

	void ShaderProgram::Use() const
	{ glUseProgram(m_ProgramID); }

	void ShaderProgram::Clear() const
	{ glUseProgram(0); }
}
