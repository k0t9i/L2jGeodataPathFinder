#pragma once

#include <cstdint>

struct Point
{
	uint32_t x;
	uint32_t y;
	bool operator<(const Point& o)  const
	{
		return x < o.x || (x == o.x && y < o.y);
	}
};