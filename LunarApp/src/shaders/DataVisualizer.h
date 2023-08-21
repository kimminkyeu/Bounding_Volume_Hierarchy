//
// Created by USER on 2023-08-10.
//

#ifndef SCOOP_DATAVISUALIZER_H
#define SCOOP_DATAVISUALIZER_H

// NOTE: Normal, Polygon, Vertex Visualizer
#include "LunarApp/src/shaders/Point/PointShader.h"
#include "LunarApp/src/shaders/Wireframe/WireframeShader.h"
#include "LunarApp/src/shaders/Normal/NormalShader.h"
#include "LunarApp/src/shaders/AABB/AABBShader.h"

class DataVisualizer
{
public:
	bool m_ShowNormal = false;
	bool m_ShowWireframe = false;
	bool m_ShowPoint = false;
	bool m_ShowAABB = false;

	Lunar::Shader* m_NormalShader = nullptr;
	Lunar::Shader* m_WireframeShader = nullptr;
	Lunar::Shader* m_PointShader = nullptr;
	Lunar::Shader* m_AABBShader = nullptr;

public:
	DataVisualizer();
    void Init();
	~DataVisualizer();
};


#endif//SCOOP_DATAVISUALIZER_H
