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

// https://en.cppreference.com/w/cpp/types/underlying_type
template <typename ENUM_CLASS_T>
inline ENUM_CLASS_T operator | (ENUM_CLASS_T lhs, ENUM_CLASS_T rhs)
{
	using T = std::underlying_type_t<ENUM_CLASS_T>;
	return static_cast<ENUM_CLASS_T>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

template <typename ENUM_CLASS_T>
inline ENUM_CLASS_T operator & (ENUM_CLASS_T lhs, ENUM_CLASS_T rhs)
{
	using T = std::underlying_type_t<ENUM_CLASS_T>;
	return static_cast<ENUM_CLASS_T>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

template <typename ENUM_CLASS_T>
inline ENUM_CLASS_T operator ^ (ENUM_CLASS_T lhs, ENUM_CLASS_T rhs)
{
	using T = std::underlying_type_t<ENUM_CLASS_T>;
	return static_cast<ENUM_CLASS_T>(static_cast<T>(lhs) ^ static_cast<T>(rhs));
}

template <typename ENUM_CLASS_T>
inline ENUM_CLASS_T& operator |= (ENUM_CLASS_T& lhs, ENUM_CLASS_T rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

template <typename ENUM_CLASS_T>
inline ENUM_CLASS_T& operator &= (ENUM_CLASS_T& lhs, ENUM_CLASS_T rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

template <typename ENUM_CLASS_T>
inline ENUM_CLASS_T& operator ^= (ENUM_CLASS_T& lhs, ENUM_CLASS_T rhs)
{
	lhs = lhs ^ rhs;
	return lhs;
}

enum class Option : u_char // max 8 bit
{
	None	     = 0,
	Wireframe    = 1 << 0, // 1
	Point        = 1 << 1, // 10
	Mesh         = 1 << 2, // 100
	Grid         = 1 << 3, // 1000
	Normal       = 1 << 4, // 10000
	AABB         = 1 << 5, // 100000
};

static Option VS_TEXTURE_PAINT_MODE_OPTION = (Option::Mesh);
static Option VS_LAYOUT_MODE_OPTION = (Option::Grid | Option::Mesh | Option::Wireframe | Option::Point);

// enum class의 타입 선언 방식이 다르기 때문에, 이 부분은 명시적으로 타입에 대한 operator를 재정의해야 함.
class DataVisualizer
{
public:
	DataVisualizer()
		: m_VisualModeBit(Option::None)
	{}

	~DataVisualizer()
	{
		delete WireframeShader;
		delete PointShader;
		delete NormalShader;
		delete m_AABBShader;
		delete m_GridShader;
	}
	// 주어진 옵션을 현재 옵션에서 추가
	inline Option AddMode(Option bit) { m_VisualModeBit |= bit; return m_VisualModeBit; }

	// 주어진 옵션으로 덮어쓰기
	inline Option OverrideMode(Option bit) { m_VisualModeBit = bit; return m_VisualModeBit; }

	inline Option ToggleMode(Option bit) { m_VisualModeBit ^= bit; return m_VisualModeBit; }

	// 해당 설정이 true 인지 체크 후 반환
	inline bool IsSet(Option bit)
	{
		// 1110 & 1010 = 1010; --> true
		// 0010 & 0100 = 0000 --> false
		// & bit의 결과가 bit와 같을 경우 true;
		return ((m_VisualModeBit & bit) == bit);
	}

	// 설정된 VAO / VBO가 없을 경우 쉐이더 생성이 안되기 때문에,
	void Init(Option bit = Option::None)
	{
		this->OverrideMode(bit);
		NormalShader = new class NormalShader();
		PointShader = new class PointShader();
		WireframeShader = new class WireframeShader();
		m_AABBShader = new class AABBShader();
		m_GridShader = new class GridShader();
	}

private:
	Option m_VisualModeBit;

public:
	Lunar::Shader* NormalShader = nullptr;
	Lunar::Shader* WireframeShader = nullptr;
	Lunar::Shader* PointShader = nullptr;
	Lunar::Shader* m_AABBShader = nullptr;
	Lunar::Shader* m_GridShader = nullptr;
};


#endif//SCOOP_DATAVISUALIZER_H
