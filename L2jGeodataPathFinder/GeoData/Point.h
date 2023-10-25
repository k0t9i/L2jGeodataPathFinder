#pragma once

#include <cstdint>

struct Point
{
	int32_t x;
	int32_t y;
	bool operator<(const Point& o)  const
	{
		return x < o.x || (x == o.x && y < o.y);
	}
};