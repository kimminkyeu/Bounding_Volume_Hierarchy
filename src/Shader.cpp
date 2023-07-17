//
// Created by Minkyeu Kim on 7/17/23.
//

#include "Shader.h"

// read file.
std::string convert_shader_file_to_string(const std::string& path)
{
	std::ifstream t(path);
	if (t.fail()) {
		return {""};
	}
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}


Shader::Shader()
{}

Shader::~Shader()
{}
