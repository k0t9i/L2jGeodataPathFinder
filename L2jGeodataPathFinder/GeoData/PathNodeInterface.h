#pragma once

#include <memory>
#include <stdint.h>
#include <vector>
#include "Point.h"

class PathNodeInterface
{
public:
	PathNodeInterface(const Point& coords) : m_Coords(coords) {};
	virtual ~PathNodeInterface() = default;

	const Point& GetCoords() const
	{
		return m_Coords;
	}
	const std::shared_ptr<PathNodeInterface> GetParent() const
	{
		return m_Parent;
	}
	void SetParent(std::shared_ptr<PathNodeInterface> parent)
	{
		m_Parent = parent;
	}
	const float GetG() const
	{
		return m_G;
	}
	void SetG(float g)
	{
		m_G = g;
	}
	const float GetH() const
	{
		return m_H;
	}
	void SetH(float h)
	{
		m_H = h;
	}
	const float GetF() const
	{
		return m_G + m_H;
	}
	virtual const float GetGToNewNode(std::shared_ptr<PathNodeInterface> other, const std::vector<std::shared_ptr<PathNodeInterface>>& neighbors) const = 0;
	virtual const int16_t GetHeight() const = 0;

private:
	const Point m_Coords;
	std::shared_ptr<PathNodeInterface> m_Parent = nullptr;
	float m_G = 0;
	float m_H = 0;
};