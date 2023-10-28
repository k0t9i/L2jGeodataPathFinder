#pragma once

#include <memory>
#include <cmath>
#include "Layer.h"
#include "PathNodeInterface.h"

class LayerDecorator : public PathNodeInterface
{
public:
    LayerDecorator(const uint16_t maxPassableHeight, const std::shared_ptr<Layer> layer, const Point& coords) :
        m_MaxPassableHeight(maxPassableHeight),
        m_Layer(layer),
        PathNodeInterface(coords)
    {
    }
    virtual ~LayerDecorator() = default;

    const float GetGToNewNode(std::shared_ptr<PathNodeInterface> other, const std::vector<std::shared_ptr<PathNodeInterface>>& neighbors) const override
    {
        const auto decorator = std::dynamic_pointer_cast<LayerDecorator>(other);

        if (GetCoords().y > decorator->GetCoords().y && !m_Layer->IsNorthOpen())
        {
            return IMPASSABLE_SCORE;
        }
        if (GetCoords().y < decorator->GetCoords().y && !m_Layer->IsSouthOpen())
        {
            return IMPASSABLE_SCORE;
        }
        if (GetCoords().x < decorator->GetCoords().x && !m_Layer->IsEastOpen())
        {
            return IMPASSABLE_SCORE;
        }
        if (GetCoords().x > decorator->GetCoords().x && !m_Layer->IsWestOpen())
        {
            return IMPASSABLE_SCORE;
        }
        if (std::abs(GetHeight() - decorator->GetHeight()) > m_MaxPassableHeight)
        {
            return IMPASSABLE_SCORE;
        }

        auto cost = GetCoords().x == decorator->GetCoords().x || GetCoords().y == decorator->GetCoords().y ? COMMON_SCORE : DIAGONAL_SCORE;

        return cost * GetNeighborsMultiplier(neighbors);
    }

    const int16_t GetHeight() const override
    {
        return m_Layer->GetHeight();
    }

private:
    const float GetNeighborsMultiplier(const std::vector<std::shared_ptr<PathNodeInterface>>& neighbors) const
    {
        if (neighbors.size() == 0) {
            return 1;
        }

        uint8_t obstaclesCount = 0;
        for (const auto& neighbor : neighbors)
        {
            const auto decorator = std::dynamic_pointer_cast<LayerDecorator>(neighbor);
            if (!decorator->m_Layer->IsCompletelyOpen()) {
                obstaclesCount++;
            }
        }

        if (obstaclesCount == 0) {
            return 1;
        }

        return (float)neighbors.size() / obstaclesCount;
    }

private:
    const uint16_t m_MaxPassableHeight;
    const std::shared_ptr<Layer> m_Layer;
    constexpr static float IMPASSABLE_SCORE = FLT_MAX / 2;
    constexpr static float COMMON_SCORE = 10;
    constexpr static float DIAGONAL_SCORE = COMMON_SCORE * 1.41421356f;
};