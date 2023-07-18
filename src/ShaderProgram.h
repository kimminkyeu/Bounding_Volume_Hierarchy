//
// Created by Minkyeu Kim on 7/17/23.
//

#ifndef SCOOP_SHADERPROGRAM_H
#define SCOOP_SHADERPROGRAM_H

#include <GL/glew.h>
#include <string> // for std::string
#include <iostream>
#include <fstream> // for ifstream
#include <sstream> // for stringstream

class ShaderProgram
{
private:
	GLuint m_ProgramID;
	GLuint m_uniformProjectionLocation;
	GLuint m_uniformModelLocation;

public:
	ShaderProgram();
	~ShaderProgram();

	// compile and attach shader to shaderProgram
	void compileAndAttachShader(const std::string& shaderPath, GLenum shaderType);

	void clearShader();

	// link shaderProgram to GPU
	void linkToGPU();

	GLuint getUniformProjectionLocation() const;
	GLuint getUniformModelLocation() const;
	GLuint getProgramID() const;

private:
	static std::string _readFileToString(const std::string& path);
};

#endif //SCOOP_SHADERPROGRAM_H
