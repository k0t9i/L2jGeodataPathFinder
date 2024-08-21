#pragma once

#include <memory>
#include <cstdint>
#include "PathNodeFactoryInterface.h"
#include "Converter.h"
#include "DataHandler.h"
#include "LayerDecorator.h"

class LayerDecoratorFactory : public PathNodeFactoryInterface
{
public:
	LayerDecoratorFactory(DataHandler& dataHandler, const uint16_t maxPassableHeight) :
		m_DataHandler(dataHandler),
		m_MaxPassableHeight(maxPassableHeight)
	{};
	virtual ~LayerDecoratorFactory()
	{
		m_DataHandler.UnloadRegionsExcept(m_StartRegionCoords, m_TargetRegionCoords);
	}

	const std::shared_ptr<PathNodeInterface> CreateStartNode(const float x, const float y, const float z) override
	{
		const auto coords = Converter::WorldCoordsToCell(x, y);
		m_StartRegionCoords = Converter::CellCoordsToRegion(coords);
		return CreateNode(coords, z);
	}

	const std::shared_ptr<PathNodeInterface> CreateTargetNode(const float x, const float y, const float z) override
	{
		const auto coords = Converter::WorldCoordsToCell(x, y);
		m_TargetRegionCoords = Converter::CellCoordsToRegion(coords);
		return CreateNode(coords, z);
	}

	const std::shared_ptr<PathNodeInterface> CreateNode(const float x, const float y, const float z) override
	{
		return CreateNode(Converter::WorldCoordsToCell(x, y), z);
	}

	const std::shared_ptr<PathNodeInterface> CreateNode(const Point& coords, const float z) override
	{
		if (m_Decorators.find(coords) == m_Decorators.end()) {
			const auto cell = m_DataHandler.GetCell(coords);
			m_Decorators[coords] = std::make_shared<LayerDecorator>(m_MaxPassableHeight, GetClosestLayer(cell, z), coords);
		}

		return m_Decorators[coords];
	}

private:
	const std::shared_ptr<Layer> GetClosestLayer(const Region::Cell& cell, const float z) const
	{
		if (cell.size() == 1)
		{
			return cell[0];
		}

		auto result = cell[0];
		float delta = std::abs(result->GetHeight() - z);

		for (const auto& layer : cell)
		{
			float currentDelta = std::abs(layer->GetHeight() - z);
			if (currentDelta < delta)
			{
				delta = currentDelta;
				result = layer;
			}
		}

		return result;
	}

private:
	DataHandler& m_DataHandler;
	const uint16_t m_MaxPassableHeight = 0;
	std::map<Point, std::shared_ptr<LayerDecorator>> m_Decorators;
	Point m_StartRegionCoords = { 0, 0 };
	Point m_TargetRegionCoords = { 0, 0 };
};