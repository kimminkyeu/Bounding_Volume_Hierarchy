//
// Created by USER on 2023-08-10.
//

#ifndef SCOOP_DATAVISUALIZER_H
#define SCOOP_DATAVISUALIZER_H

// NOTE: Normal, Polygon, Vertex Visualizer
#include "LunarApp/src/Shaders/Point/PointShader.h"
#include "LunarApp/src/Shaders/Wireframe/WireframeShader.h"
#include "LunarApp/src/Shaders/Normal/NormalShader.h"
#include "LunarApp/src/Shaders/AABB/AABBShader.h"
#include "LunarApp/src/Shaders/Grid/GridShader.h"

class DataVisualizer
{
public:
	bool m_ShowNormal = false;
	bool m_ShowWireframe = true;
	bool m_ShowPoint = false;
	bool m_ShowAABB = false;
	bool m_ShowGrid = true;

	Lunar::Shader* m_NormalShader = nullptr;
	Lunar::Shader* m_WireframeShader = nullptr;
	Lunar::Shader* m_PointShader = nullptr;
	Lunar::Shader* m_AABBShader = nullptr;
	Lunar::Shader* m_GridShader = nullptr;

public:
	DataVisualizer();
    void Init();
	~DataVisualizer();
};


#endif//SCOOP_DATAVISUALIZER_H
