//
// Created by USER on 2023-08-10.
//

#ifndef SCOOP_DATAVISUALIZER_H
#define SCOOP_DATAVISUALIZER_H

// NOTE: Normal, Polygon, Vertex Visualizer
#include "LunarApp/src/shaders/Point/PointShader.h"
#include "LunarApp/src/shaders/Wireframe/WireframeShader.h"
#include "LunarApp/src/shaders/Normal/NormalShader.h"

class DataVisualizer
{
public:
	bool m_ShowNormal = false;
	bool m_ShowPolygon = false;
	bool m_ShowVertices = false;

	Lunar::Shader* m_NormalShader = nullptr;
	Lunar::Shader* m_PolygonShader = nullptr;
	Lunar::Shader* m_VerticesShader = nullptr;

public:
	DataVisualizer();
    void Init();
	~DataVisualizer();
};


#endif//SCOOP_DATAVISUALIZER_H
