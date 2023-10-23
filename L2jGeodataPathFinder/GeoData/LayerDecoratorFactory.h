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
	LayerDecoratorFactory(DataHandler& dataHandler) :
		m_DataHandler(dataHandler)
	{};
	virtual ~LayerDecoratorFactory() = default;

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

	void SetMaxPassableHeight(const uint16_t maxPassableHeight)
	{
		m_MaxPassableHeight = maxPassableHeight;
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
	uint16_t m_MaxPassableHeight = 0;
	std::map<Point, std::shared_ptr<LayerDecorator>> m_Decorators;
};