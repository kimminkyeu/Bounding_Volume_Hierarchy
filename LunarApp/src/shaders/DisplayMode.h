//
// Created by USER on 2023-08-04.
//

#ifndef SCOOP_DISPLAYMODE_H
#define SCOOP_DISPLAYMODE_H

#include "Lunar/Shader/Shader.h"
#include <map>

// http://docs.mcneel.com/rhino/5/help/en-us/options/view_displaymode_options.htm

class DisplayMode
{
private:
	std::map< std::string, Lunar::Shader* > m_ShaderMap;
	Lunar::Shader* m_CurrentShader = nullptr;

public:
	DisplayMode();
	~DisplayMode();
	DisplayMode& operator=(const DisplayMode&) = delete;
	DisplayMode(const DisplayMode&) = delete;
public:

	// compile + register shader program
	void Add(const std::string& name,
			 const std::string& vetexShaderPath,
			 const std::string& fragmentShaderPath,
			 const std::string& geometryShaderPath = ""
			 );

	// controller.Add( new ExplosionShader() );
	inline void Add(Lunar::Shader* shaderPtr)
	{ this->m_ShaderMap.insert( {shaderPtr->GetName(), shaderPtr}); };

	// get program
	Lunar::Shader* GetByName(const std::string& name);
	Lunar::Shader* GetCurrentShaderPtr();
	std::map< std::string, Lunar::Shader* > GetShaderMap();

	void SetCurrentShader(const std::string& name);
	void BindCurrentShader();
	inline void UnbindCurrentShader() { m_CurrentShader->Unbind(); }

	// update current shader variables to GPU
//	void OnUpdate() { m_CurrentShader->BindDataToGPU(); }
};

#endif//SCOOP_DISPLAYMODE_H
