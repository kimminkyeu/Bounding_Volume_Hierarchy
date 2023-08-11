//
// Created by USER on 2023-08-04.
//

#include "DisplayMode.h"
#include "Lunar/Core/Log.h"

DisplayMode::DisplayMode()
{}

DisplayMode::~DisplayMode()
{
	for (auto& itr : m_ShaderMap)
		delete itr.second;
	m_ShaderMap.clear();
	LOG_TRACE("ShaderController destructor called");
}

void DisplayMode::BindCurrentShader()
{
	assert(!m_ShaderMap.empty() && "No shader has been provided");
	if (m_CurrentShader == nullptr)
	{
		m_CurrentShader = this->GetByName("Phong");
//		auto itr = m_ShaderMap.begin();
//		m_CurrentShader = itr->second;
	}
	m_CurrentShader->Bind();
}

void DisplayMode::SetCurrentShader(const std::string& name)
{
	auto f = m_ShaderMap.find(name);
	if (f != m_ShaderMap.end()) {
		m_CurrentShader = f->second;
		f->second->Bind();
	} else {
		assert(false && "ShaderController::BindCurrentShader() -> shader category not found");
	}
}

void DisplayMode::Add(const std::string& name,
						   const std::string& vertexShaderPath,
						   const std::string& fragmentShaderPath,
						   const std::string& geometryShaderPath // optional
						   )
{
	auto* shader_ptr = new Lunar::Shader(name, vertexShaderPath, fragmentShaderPath, geometryShaderPath);
	m_ShaderMap.insert(std::make_pair(name, shader_ptr));
}

Lunar::Shader* DisplayMode::GetByName(const std::string& name)
{
	auto itr = m_ShaderMap.find(name);
	if (itr != m_ShaderMap.end()) {
		return itr->second;
	} else {
		return nullptr;
	}
}
Lunar::Shader* DisplayMode::GetCurrentShaderPtr()
{
	return m_CurrentShader;
}
std::map<std::string, Lunar::Shader*> DisplayMode::GetShaderMap()
{
	return m_ShaderMap;
}
