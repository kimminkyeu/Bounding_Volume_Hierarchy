//
// Created by Minkyeu Kim on 7/17/23.
//

#ifndef SCOOP_SHADER_H
#define SCOOP_SHADER_H

#include <GL/glew.h>
#include <string> // for std::string
#include <fstream> // for ifstream
#include <sstream> // for stringstream

class ShaderGroup
{
private:
	GLuint m_ShaderID;
	GLuint m_uniformProjectionMatrix;
	GLuint m_uniformModelMatrix;

public:
	Shader();
	~Shader();
	GLuint getProjectionMatrixLocation();
	GLuint getModelMatrixLocation();
	void useShader();
	void clearShader();
	void readSourceFromFile(const std::string& path_to_shader);

private:
	void addShader(GLuint theProgram, const std::string& shader_code, GLenum shaderType);
	void compileShader();
};

#endif //SCOOP_SHADER_H
