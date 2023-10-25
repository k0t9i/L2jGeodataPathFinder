#pragma once

#include <memory>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include "Layer.h"
#include "Point.h"
#include "Constants.h"

class Region
{
public:
	using Cell = std::vector<std::shared_ptr<Layer>>;

	Region(const uint8_t col, const uint8_t row) : m_Col(col), m_Row(row) {};
	virtual ~Region() = default;

	void AddCell(const Cell& cell)
	{
		const auto size = m_Cells.size();

		const auto blockCount = (uint16_t)std::ceil((float)(size + 1) / (Constants::CELLS_DIMENSION * Constants::CELLS_DIMENSION));
		const auto blockX = (uint8_t)std::floor((float)(blockCount - 1) / Constants::BLOCKS_DIMENSION);
		const uint8_t blockY = (blockCount - 1) - blockX * Constants::BLOCKS_DIMENSION;
		const auto cellX = (uint8_t)std::floor((float)size / Constants::CELLS_DIMENSION) % Constants::CELLS_DIMENSION;
		const uint8_t cellY = (size - cellX * Constants::CELLS_DIMENSION) % Constants::CELLS_DIMENSION;
		int32_t x = blockX * Constants::CELLS_DIMENSION + cellX;
		int32_t y = blockY * Constants::CELLS_DIMENSION + cellY;
		x += Constants::REGION_CELLS_DIMENSION * m_Col;
		y += Constants::REGION_CELLS_DIMENSION * m_Row;

		m_Cells[{ x, y }] = cell;
	}

	const uint8_t GetCol() const
	{
		return m_Col;
	}

	const uint8_t GetRow() const
	{
		return m_Row;
	}

	const Cell& GetCell(Point colAndRow) const
	{
		return m_Cells.at(colAndRow);
	}

	const std::map<Point, Cell>& GetCells() const
	{
		return m_Cells;
	}

private:
	std::map<Point, Cell> m_Cells;
	const uint8_t m_Col;
	const uint8_t m_Row;
};