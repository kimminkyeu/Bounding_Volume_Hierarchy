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
#include <cassert>

class ShaderProgram
{
private:
	GLuint m_ProgramID;
	GLuint m_uniformProjectionLocation;
	GLuint m_uniformModelLocation;

public:
	ShaderProgram();
	ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
	~ShaderProgram();

	// compile and attach shader to shaderProgram
	int attachShader(const std::string& shaderPath, GLenum shaderType);
	// link shaderProgram to GPU + load uniform variable location
	int linkToGPU();
	// change member uniform variable. (Projection)
	void setUniformModel(/* glm mat 4 */);
	// change member uniform variable. (Projection)
	void setUniformProjection(/* glm mat 4 */);
	// delete program.
	void clearShader();

	GLuint getUniformProjectionLocation() const;
	GLuint getUniformModelLocation() const;
	GLuint getProgramID() const;

private:
	static std::string _readFileToString(const std::string& path);
};

#endif //SCOOP_SHADERPROGRAM_H
