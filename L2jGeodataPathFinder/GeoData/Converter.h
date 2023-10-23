#pragma once

#include <cmath>
#include "Point.h"
#include "Constants.h"

class Converter
{
public:
	static const Point WorldCoordsToCell(const float x, const float y)
	{
		return Point
		{
			(uint32_t)std::floor((x / Constants::BLOCK_SIZE_IN_GAME_COORDS + Constants::DELTA_COL) * Constants::REGION_CELLS_DIMENSION),
			(uint32_t)std::floor((y / Constants::BLOCK_SIZE_IN_GAME_COORDS + Constants::DELTA_ROW) * Constants::REGION_CELLS_DIMENSION)
		};
	}

	static const Point CellCoordsToRegion(const Point& coords)
	{
		return Point
		{
			(uint32_t)std::floor(coords.x / Constants::REGION_CELLS_DIMENSION),
			(uint32_t)std::floor(coords.y / Constants::REGION_CELLS_DIMENSION)
		};
	}

	static const Point CellCoordsToWorld(const Point& coords)
	{
		return Point
		{
			(coords.x - Constants::DELTA_COL * Constants::REGION_CELLS_DIMENSION) * Constants::CELL_SIZE_IN_GAME_COORDS,
			(coords.y - Constants::DELTA_ROW * Constants::REGION_CELLS_DIMENSION) * Constants::CELL_SIZE_IN_GAME_COORDS,
		};
	}
};