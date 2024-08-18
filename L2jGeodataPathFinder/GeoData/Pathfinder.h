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

        if (LineOfSight(start, target)) {
            result.push_back(start);
            result.push_back(target);

            return result;
        }

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

                return SmoothPath(result);
            }

            m_Open.erase(current->GetCoords());
            m_Closed[current->GetCoords()] = current;

            for(const auto node : GetNeighbors(current))
            {
                if (m_Closed.find(node->GetCoords()) != m_Closed.end())
                {
                    continue;
                }

                const auto newGScore = current->GetG() + current->GetGToNewNode(node, GetNeighbors(current, 3));
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

    const bool LineOfSight(const float startX, const float startY, float startZ, const float endX, const float endY) const
    {
        const auto start = m_PathNodeFactory.CreateNode(startX, startY, startZ);
        const auto target = m_PathNodeFactory.CreateNode(endX, endY, startZ);

        return LineOfSight(start, target);
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

    const std::vector <std::shared_ptr<PathNodeInterface>> SmoothPath(const std::vector <std::shared_ptr<PathNodeInterface>>& path) const
    {
        std::vector<std::shared_ptr<PathNodeInterface>> result;

        if (path.size() == 0) {
            return result;
        }

        auto current = path[0];
        result.push_back(current);

        size_t i = 1;
        for (size_t i = 1; i < path.size(); i++)
        {
            const auto next = path[i];

            if (LineOfSight(current, next)) {
                continue;
            }
            else
            {
                current = next;
                result.push_back(path[i - 1]);
            }
        }

        result.push_back(path.back());

        return result;
    }

    const bool LineOfSight(std::shared_ptr<PathNodeInterface> start, std::shared_ptr<PathNodeInterface> end) const
    {
        const auto& path = GetStraightPath(start, end);

        for (size_t i = 0; i < path.size() - 1; i++)
        {
            const auto& current = path[i];
            const auto& next = path[i + 1];
            if (!current->CanMoveTo(next)) {
                return false;
            }
        }

        return true;
    }

    const std::vector<std::shared_ptr<PathNodeInterface>> GetStraightPath(std::shared_ptr<PathNodeInterface> start, std::shared_ptr<PathNodeInterface> end) const
    {
        const auto xS = start->GetCoords().x;
        const auto yS = start->GetCoords().y;
        const auto xE = end->GetCoords().x;
        const auto yE = end->GetCoords().y;

        int32_t signX = sign(xE - xS);
        int32_t signY = sign(yE - yS);

        std::vector<std::shared_ptr<PathNodeInterface>> result;


        int32_t x0 = 0;
        int32_t y0 = 0;
        int32_t x1 = xE - xS;
        int32_t y1 = yE - yS;

        float k = std::abs((float)(y1 - y0) / (x1 - x0));

        int32_t x = x0;
        int32_t y = y0;

        result.push_back(m_PathNodeFactory.CreateNode({ x + xS, y + yS }, start->GetHeight()));
        while (x != x1 || y != y1)
        {
            float t = (float)(2 * y * signY + 1) / (2 * x * signX + 1);
            if (t >= k)
            {
                x += signX;
            }
            if (t <= k)
            {
                y += signY;
            }
            result.push_back(m_PathNodeFactory.CreateNode({ x + xS, y + yS }, start->GetHeight()));
        }

        return result;
    }

    const int32_t sign(const int32_t val) const
    {
        return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
    }
private:
    PathNodeFactoryInterface& m_PathNodeFactory;
	std::map<Point, std::shared_ptr<PathNodeInterface>> m_Closed;
	std::map<Point, std::shared_ptr<PathNodeInterface>> m_Open;
    const static uint8_t NEIGHBORS_COUNT = 3;
};