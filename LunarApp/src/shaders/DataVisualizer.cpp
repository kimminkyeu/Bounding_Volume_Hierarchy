//
// Created by USER on 2023-08-10.
//

#include "DataVisualizer.h"
DataVisualizer::DataVisualizer()
{
}

DataVisualizer::~DataVisualizer()
{
	delete m_WireframeShader;
	delete m_PointShader;
	delete m_NormalShader;
	delete m_AABBShader;
}

void DataVisualizer::Init()
{
    m_NormalShader = new NormalShader();
	m_PointShader = new PointShader();
	m_WireframeShader = new WireframeShader();
	m_AABBShader = new AABBShader();
}
