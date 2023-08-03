//
// Created by USER on 2023-08-04.
//

#ifndef SCOOP_SHADERCONTROLLER_H
#define SCOOP_SHADERCONTROLLER_H

#include <map>
#include "Lunar/Shader/ShaderProgram.h"

// http://docs.mcneel.com/rhino/5/help/en-us/options/view_displaymode_options.htm
enum class eShaderCategory : uint8_t
{
	Test, // use xyz position value as color
	// Default Shading
	Flat,
	Gouraud,
	Phong,
	// Rhino 3D Display
	WireFrame, // only vertex frame
	Shaded, // wireframe + ambient + diffuse shading (no Specular)
	Cartoon,
};

class ShaderController
{
private:
	std::map< std::string, Lunar::ShaderProgram* > m_ShaderProgramMap;
	Lunar::ShaderProgram* m_SelectedShader = nullptr;

public:
	ShaderController();
	~ShaderController();
	ShaderController& operator=(const ShaderController&) = delete;
	ShaderController(const ShaderController&) = delete;
public:
	// compile + register shader program
	void Add(const std::string& name, const std::string& vetexShaderPath, const std::string& fragmentShaderPath);
	// get program
	Lunar::ShaderProgram* GetByName(const std::string& name);
	Lunar::ShaderProgram* GetCurrentShader();
	std::map< std::string, Lunar::ShaderProgram* > GetShaderProgramMap();

	void Use(const std::string& name);
	void Use();
};

#endif//SCOOP_SHADERCONTROLLER_H
