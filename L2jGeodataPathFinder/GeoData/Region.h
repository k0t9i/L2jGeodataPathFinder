#pragma once

#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
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
		if (m_Cells.empty())
		{
			m_Cells.reserve(Constants::REGION_CELLS_DIMENSION * Constants::REGION_CELLS_DIMENSION);
		}

		m_Cells.push_back(cell);
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
		const auto x = colAndRow.x - Constants::REGION_CELLS_DIMENSION * m_Col;
		const auto y = colAndRow.y - Constants::REGION_CELLS_DIMENSION * m_Row;

		if (x < 0 || x >= Constants::REGION_CELLS_DIMENSION || y < 0 || y >= Constants::REGION_CELLS_DIMENSION)
		{
			throw std::out_of_range("Cell coordinates are outside the region");
		}

		const auto blockX = x / Constants::CELLS_DIMENSION;
		const auto blockY = y / Constants::CELLS_DIMENSION;
		const auto cellX = x % Constants::CELLS_DIMENSION;
		const auto cellY = y % Constants::CELLS_DIMENSION;
		const auto index = (blockX * Constants::BLOCKS_DIMENSION + blockY) * Constants::CELLS_DIMENSION * Constants::CELLS_DIMENSION + cellX * Constants::CELLS_DIMENSION + cellY;

		return m_Cells.at(index);
	}

	const std::vector<Cell>& GetCells() const
	{
		return m_Cells;
	}

private:
	std::vector<Cell> m_Cells;
	const uint8_t m_Col;
	const uint8_t m_Row;
};