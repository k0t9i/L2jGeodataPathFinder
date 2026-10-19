#pragma once

#include <memory>
#include <cmath>
#include <map>
#include <queue>
#include <vector>
#include "PathNodeFactoryInterface.h"
#include "PathNodeInterface.h"
#include "Point.h"
#include "Constants.h"

class Pathfinder
{
public:
    Pathfinder(PathNodeFactoryInterface& pathNodeFactory) : m_PathNodeFactory(pathNodeFactory) {};
	virtual ~Pathfinder() = default;

    const std::vector<std::shared_ptr<PathNodeInterface>> FindPath(const float startX, const float startY, float startZ, const float endX, const float endY, float range)
    {
        const auto start = m_PathNodeFactory.CreateStartNode(startX, startY, startZ);
        const auto target = m_PathNodeFactory.CreateTargetNode(endX, endY, startZ);
        const auto rangeInCells = std::max(0.0f, range) / Constants::CELL_SIZE_IN_GAME_COORDS;

        std::vector<std::shared_ptr<PathNodeInterface>> result;

        if (IsWithinRange(start, target, rangeInCells))
        {
            result.push_back(start);
            return result;
        }

        const auto straightPath = GetStraightPath(start, target);

        for (size_t i = 1; i < straightPath.size(); i++)
        {
            if (!straightPath[i - 1]->CanMoveTo(straightPath[i]))
            {
                break;
            }

            if (IsWithinRange(straightPath[i], target, rangeInCells))
            {
                result.push_back(start);
                result.push_back(rangeInCells == 0.0f ? target : straightPath[i]);
                return result;
            }
        }

        m_Closed.clear();
        m_Open.clear();
        m_OpenQueue = {};

        m_Open[start->GetCoords()] = start;

        start->SetG(0);
        start->SetH(GetHeuristicCost(start, target, rangeInCells));
        m_OpenQueue.push({ start->GetF(), start });

        while (!m_OpenQueue.empty() && m_Closed.size() < MAX_VISITED_NODES)
        {
            const auto entry = m_OpenQueue.top();
            m_OpenQueue.pop();

            const auto current = entry.node;

            if (entry.f != current->GetF() || m_Closed.find(current->GetCoords()) != m_Closed.end())
            {
                continue;
            }

            if (IsWithinRange(current, target, rangeInCells))
            {
                auto currentNode = current;

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

            for (const auto node : GetNeighbors(current))
            {
                if (m_Closed.find(node->GetCoords()) != m_Closed.end())
                {
                    continue;
                }

                if (!current->CanMoveTo(node))
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
                    node->SetH(GetHeuristicCost(node, target, rangeInCells));
                    m_OpenQueue.push({ node->GetF(), node });
                }
            }
        }

        return result;
    }

    const bool HasLineOfSight(const float startX, const float startY, float startZ, const float endX, const float endY, const float endZ) const
    {
        const auto start = m_PathNodeFactory.CreateStartNode(startX, startY, startZ);
        const auto target = m_PathNodeFactory.CreateTargetNode(endX, endY, endZ);

        return CanMoveDirectly(start, target);
    }

private:
    const bool IsWithinRange(std::shared_ptr<PathNodeInterface> current, std::shared_ptr<PathNodeInterface> target, float rangeInCells) const
    {
        const auto dx = static_cast<float>(current->GetCoords().x - target->GetCoords().x);
        const auto dy = static_cast<float>(current->GetCoords().y - target->GetCoords().y);

        return dx * dx + dy * dy <= rangeInCells * rangeInCells;
    }

    const float GetHeuristicCost(std::shared_ptr<PathNodeInterface> current, std::shared_ptr<PathNodeInterface> target, float rangeInCells) const
    {
        const auto dx = static_cast<float>(current->GetCoords().x - target->GetCoords().x);
        const auto dy = static_cast<float>(current->GetCoords().y - target->GetCoords().y);

        const auto distance = std::sqrt(dx * dx + dy * dy);

        return 10.0f * std::max(0.0f, distance - rangeInCells);
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

            if (CanMoveDirectly(current, next)) {
                continue;
            }
            else
            {
                current = path[i - 1];
                result.push_back(current);
            }
        }

        result.push_back(path.back());

        return result;
    }

    const bool CanMoveDirectly(std::shared_ptr<PathNodeInterface> start, std::shared_ptr<PathNodeInterface> end) const
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
    struct OpenNode
    {
        float f;
        std::shared_ptr<PathNodeInterface> node;
    };

    struct OpenNodeCompare
    {
        bool operator()(const OpenNode& left, const OpenNode& right) const
        {
            if (left.f != right.f)
            {
                return left.f > right.f;
            }

            return right.node->GetCoords() < left.node->GetCoords();
        }
    };

    PathNodeFactoryInterface& m_PathNodeFactory;
	std::map<Point, std::shared_ptr<PathNodeInterface>> m_Closed;
	std::map<Point, std::shared_ptr<PathNodeInterface>> m_Open;
    std::priority_queue<OpenNode, std::vector<OpenNode>, OpenNodeCompare> m_OpenQueue;
    const static uint8_t NEIGHBORS_COUNT = 3;
    const static size_t MAX_VISITED_NODES = 250000;
};