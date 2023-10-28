#pragma once

#include <memory>
#include <cmath>
#include <map>
#include <vector>
#include "PathNodeFactoryInterface.h"
#include "PathNodeInterface.h"
#include "Point.h"

class Pathfinder
{
public:
    Pathfinder(PathNodeFactoryInterface& pathNodeFactory) : m_PathNodeFactory(pathNodeFactory) {};
	virtual ~Pathfinder() = default;

    const std::vector<std::shared_ptr<PathNodeInterface>> FindPath(const float startX, const float startY, float startZ, const float endX, const float endY)
    {
        const auto start = m_PathNodeFactory.CreateNode(startX, startY, startZ);
        const auto target = m_PathNodeFactory.CreateNode(endX, endY, startZ);

        std::vector<std::shared_ptr<PathNodeInterface>> result;

        m_Closed.clear();
        m_Open.clear();

        m_Open[start->GetCoords()] = start;

        start->SetG(0);
        start->SetH(GetHeuristicCost(start, target));

        while (m_Open.size() > 0)
        {
            const auto current = GetFittestOpenNode();

            if (current == target)
            {
                auto currentNode = target;
                while (currentNode != nullptr)
                {
                    result.push_back(currentNode);
                    currentNode = currentNode->GetParent();
                }

                std::reverse(result.begin(), result.end());

                return result;
            }

            m_Open.erase(current->GetCoords());
            m_Closed[current->GetCoords()] = current;

            for(const auto node : GetNeighbors(current))
            {
                if (m_Closed.find(node->GetCoords()) != m_Closed.end())
                {
                    continue;
                }

                const auto newGScore = current->GetG() + current->GetGToNewNode(node, GetNeighbors(current, NEIGHBORS_COUNT));
                auto isNewBetter = true;

                if (m_Open.find(node->GetCoords()) == m_Open.end())
                {
                    m_Open[node->GetCoords()] = node;
                }
                else if (newGScore >= node->GetG())
                {
                    isNewBetter = false;
                }

                if (isNewBetter)
                {
                    node->SetParent(current);
                    node->SetG(newGScore);
                    node->SetH(GetHeuristicCost(node, target));
                }
            }
        }

        return result;
    }

private:
	const float GetHeuristicCost(std::shared_ptr<PathNodeInterface> current, std::shared_ptr<PathNodeInterface> target) const
	{
        const auto cost = std::abs((float)((int32_t)target->GetCoords().x - (int32_t)current->GetCoords().x)) + std::abs((float)((int32_t)target->GetCoords().y - (int32_t)current->GetCoords().y));
        return 10 * cost;
	}

	const std::shared_ptr<PathNodeInterface> GetFittestOpenNode() const
	{
		float minF = FLT_MAX;
		std::shared_ptr<PathNodeInterface> result = m_Open.begin()->second;

		for (const auto& kvp : m_Open)
		{
			if (kvp.second->GetF() < minF)
			{
				minF = kvp.second->GetF();
				result = kvp.second;
			}
		}

		return result;

	}
	
	const std::vector<std::shared_ptr<PathNodeInterface>> GetNeighbors(const std::shared_ptr<PathNodeInterface> node, const uint8_t radius = 1) const
	{
        std::vector<std::shared_ptr<PathNodeInterface>> result;
        const auto& coords = node->GetCoords();

        for (int32_t x = coords.x - radius; x <= coords.x + radius; x++)
        {
            for (int32_t y = coords.y - radius; y <= coords.y + radius; y++)
            {
                if (x == coords.x && y == coords.y)
                {
                    continue;
                }

                result.push_back(m_PathNodeFactory.CreateNode({x, y}, node->GetHeight()));
            }
        }

        return result;
	}
private:
    PathNodeFactoryInterface& m_PathNodeFactory;
	std::map<Point, std::shared_ptr<PathNodeInterface>> m_Closed;
	std::map<Point, std::shared_ptr<PathNodeInterface>> m_Open;
    const static uint8_t NEIGHBORS_COUNT = 3;
};