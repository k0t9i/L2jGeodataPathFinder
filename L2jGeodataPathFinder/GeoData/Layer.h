#pragma once

#include <stdint.h>

class Layer
{
public:
	Layer(const int16_t height) : Layer(height, true, true, true, true) {};
	Layer(const int16_t height, const bool north, const bool south, const bool west, const bool east) :
		m_Height(height),
		m_North(north),
		m_South(south),
		m_West(west),
		m_East(east)
	{

	}
	virtual ~Layer() = default;

	const bool IsNorthOpen() const
	{
		return m_North;
	}
	const bool IsSouthOpen() const
	{
		return m_South;
	}
	const bool IsWestOpen() const
	{
		return m_West;
	}
	const bool IsEastOpen() const
	{
		return m_East;
	}
	const int16_t GetHeight() const
	{
		return m_Height;
	}

private:
	const int16_t m_Height = 0;
	const bool m_North = true;
	const bool m_South = true;
	const bool m_West = true;
	const bool m_East = true;
};