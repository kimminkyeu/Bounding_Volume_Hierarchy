//
// Created by Minkyeu Kim on 7/17/23.
//

#include "Shader.h"
#include "Lunar/Core/Log.h"

// vertex_shader = 0000^0001
// fragment_shader = 0000^0010
static const char ESSENTIAL_SHADERS_LOADED = (1 << 1) | (1 << 0);

namespace Lunar {

	Shader::Shader(const std::string& name)
		: m_DebugName(name), m_ProgramID(0), m_UniformModelLocation(-1), m_UniformProjectionLocation(-1), m_UniformViewLocation(-1), m_ShadersBitmap(0)
	{}

	Shader::Shader(const std::string& name,
								 const std::string& vertex_shader_path,
								 const std::string& fragment_shader_path,
								 const std::string& geometry_shader_path // optional
								 )
		: Shader::Shader(name)
	{
		// 1. Load shader
		LOG_TRACE("ShaderProgram constructor called");
		std::string vShaderPath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(vertex_shader_path);
		LOG_TRACE("vertex shader path: {0}", vShaderPath);
		this->AttachShader(vShaderPath, GL_VERTEX_SHADER);
		std::string fShaderPath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(fragment_shader_path);
		LOG_TRACE("fragment shader path: {0}", fShaderPath);
		this->AttachShader(fShaderPath, GL_FRAGMENT_SHADER);
		if (!geometry_shader_path.empty()) {
			std::string gShaderPath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(geometry_shader_path);
			LOG_TRACE("geometry shader path: {0}", gShaderPath);
			this->AttachShader(gShaderPath, GL_GEOMETRY_SHADER);
		}

		// 2. compile shader
		this->LinkToGPU();
	}

	Shader::~Shader()
	{
		LOG_TRACE("ShaderProgram [{0}] destructor called", m_DebugName);
		this->DeleteFromGPU();
	}

	int Shader::AttachShader(const std::string& shaderPath, GLenum shaderType)
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
	int Shader::LinkToGPU()
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
		// TODO: 아래 uniform 변수 접근은 Phong 쉐이더 헤더로 뺄 것
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

	void Shader::SetUniformModel(const GLfloat *value) const
	{ glUniformMatrix4fv(m_UniformModelLocation, 1, GL_FALSE, value); }

	void Shader::SetUniformProjection(const GLfloat *value) const
	{ glUniformMatrix4fv(m_UniformProjectionLocation, 1, GL_FALSE, value); }

	void Shader::SetUniformView(const GLfloat *value) const
	{ glUniformMatrix4fv(m_UniformViewLocation, 1, GL_FALSE, value); }

	void Shader::SetUniformEyePos(const glm::vec3& eyePos) const
	{ glUniform3f(m_UniformEyePosLocation, eyePos.x, eyePos.y, eyePos.z); }

	void Shader::DeleteFromGPU()
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

	GLint Shader::_GetUniformLocation(const char* uniformName) const
	{
		GLint location = glGetUniformLocation(m_ProgramID, uniformName);
		if (location < 0)
		{
			LOG_WARN("  [{0} shader]: Uniform [{1}] not found.", m_DebugName, uniformName);
		}
		return location;
	}

	std::string Shader::_ReadFileToString(const std::string& path)
	{
		std::ifstream t(path);
		if (t.fail()) {
			return {""};
		}
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}

	void Shader::Bind()
	{
		glUseProgram(m_ProgramID);
		this->OnBind(); // NOTE: 각 derived shader class에서 정의한 함수 호출
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
		this->OnUnbind(); // NOTE: 각 derived shader class에서 정의한 함수 호출
	}
}
