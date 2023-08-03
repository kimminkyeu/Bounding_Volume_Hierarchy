//
// Created by USER on 2023-08-04.
//

#include "ShaderController.h"
#include "Lunar/Core/Log.h"

ShaderController::ShaderController()
{}

ShaderController::~ShaderController()
{
	for (auto& itr : m_ShaderProgramMap)
		delete itr.second;
	m_ShaderProgramMap.clear();
	LOG_TRACE("ShaderController destructor called");
}

void ShaderController::Use()
{
	assert(!m_ShaderProgramMap.empty() && "No shader has been provided");
	if (m_SelectedShader == nullptr)
	{
		auto itr = m_ShaderProgramMap.begin();
		m_SelectedShader = itr->second;
	}
	m_SelectedShader->Use();
}

void ShaderController::Use(const std::string& name)
{
	auto f = m_ShaderProgramMap.find(name);
	if (f != m_ShaderProgramMap.end()) {
		f->second->Use();
		m_SelectedShader = f->second;
	} else {
		assert(false && "ShaderController::Use() -> shader category not found");
	}
}

void ShaderController::Add(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	auto* shader_ptr = new Lunar::ShaderProgram(name, vertexShaderPath, fragmentShaderPath);
	m_ShaderProgramMap.insert(std::make_pair(name, shader_ptr));
}

Lunar::ShaderProgram* ShaderController::GetByName(const std::string& name)
{
	auto itr = m_ShaderProgramMap.find(name);
	if (itr != m_ShaderProgramMap.end()) {
		return itr->second;
	} else {
		return nullptr;
	}
}
Lunar::ShaderProgram* ShaderController::GetCurrentShader()
{
	return m_SelectedShader;
}
std::map<std::string, Lunar::ShaderProgram*> ShaderController::GetShaderProgramMap()
{
	return m_ShaderProgramMap;
}
