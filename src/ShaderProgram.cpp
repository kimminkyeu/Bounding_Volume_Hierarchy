//
// Created by Minkyeu Kim on 7/17/23.
//

#include "ShaderProgram.h"



ShaderProgram::ShaderProgram()
	: m_ProgramID(0), m_uniformModelLocation(0), m_uniformProjectionLocation(0)
{
	m_ProgramID = glCreateProgram();
	if (!m_ProgramID)
	{
		// ...
		return;
	}
}

ShaderProgram::~ShaderProgram()
{}

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

void ShaderProgram::compileAndAttachShader(const std::string& shaderPath, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	std::string shader_code = this->_readFileToString(shaderPath);
	const GLint len = shader_code.length();
	const char* src = shader_code.c_str();
	glShaderSource(theShader, 1, &src, &len);
	glCompileShader(theShader);
	GLint result = 0;
	GLchar eLog[1024] = { 0, };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), nullptr, eLog);
		std::cout << "[glGetShaderInfoLog]" << eLog << "\n";
		return ;
	}
	glAttachShader(this->m_ProgramID, theShader);
}

// 나중에 glCreateProgram 이 코드 다르게 만들기.
void ShaderProgram::linkToGPU()
{
	GLint result = 0;
	GLchar eLog[1024] = {0,};

	glLinkProgram(this->m_ProgramID);
	glGetProgramiv(this->m_ProgramID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(this->m_ProgramID, sizeof(eLog), nullptr, eLog);
		std::cerr << "Error linking program: " << eLog << "\n";
		return;
	}

	glValidateProgram(this->m_ProgramID);
	glGetProgramiv(this->m_ProgramID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		memset(eLog, 0, sizeof(eLog));
		glGetProgramInfoLog(this->m_ProgramID, sizeof(eLog), nullptr, eLog);
		std::cerr << "Error validating program: " << eLog << "\n";
		return;
	}
	// find uniform variable inside the program. then, save its ID to member variable m_*.
	m_uniformProjectionLocation = glGetUniformLocation(m_ProgramID, "projection");
	m_uniformModelLocation = glGetUniformLocation(m_ProgramID, "model");
}

GLuint ShaderProgram::getUniformModelLocation() const
{ return m_uniformModelLocation; }

GLuint ShaderProgram::getUniformProjectionLocation() const
{ return m_uniformProjectionLocation; }

GLuint ShaderProgram::getProgramID() const
{ return this->m_ProgramID; }

void ShaderProgram::clearShader()
{
	// delete program from GPU memory
	if (m_ProgramID)
	{
		glDeleteProgram(m_ProgramID);
		m_ProgramID = 0;
	}
	m_uniformModelLocation = 0;
	m_uniformProjectionLocation = 0;
}