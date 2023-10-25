#include "pch.h"
#include "framework.h"
#include "L2jGeodataPathFinder.h"
#include "GeoData/Pathfinder.h"
#include "GeoData/DataHandler.h"
#include "GeoData/LayerDecoratorFactory.h"
#include "GeoData/Converter.h"

DataHandler handler;

extern "C" L2JGEODATAPATHFINDER_API size_t FindPath(PathNode ** result, const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, const uint16_t maxPassableHeight, const bool onlyTurningPoints)
{
    handler.SetDataDirectory(geoDataDir);
    LayerDecoratorFactory factory(handler, maxPassableHeight);
    Pathfinder pathfinder(factory);

    auto path = pathfinder.FindPath(startX, startY, startZ, endX, endY);
    if (onlyTurningPoints) {
        path = ReducePath(path);
    }

    *result = ConvertPath(path);

    return path.size();
}

extern "C" L2JGEODATAPATHFINDER_API void ReleasePath(PathNode * path)
{
    delete[] path;
}

const std::vector<std::shared_ptr<PathNodeInterface>> ReducePath(const std::vector<std::shared_ptr<PathNodeInterface>> nodes)
{
    std::vector<std::shared_ptr<PathNodeInterface>> result;

    int8_t prevDx = 0;
    int8_t prevDy = 0;
    for (size_t i = 1; i < nodes.size(); i++)
    {
        const auto& prevNode = nodes[i - 1];
        const auto& node = nodes[i];

        int8_t dx = (int)(node->GetCoords().x - prevNode->GetCoords().x);
        int8_t dy = (int)(node->GetCoords().y - prevNode->GetCoords().y);

        if (dx != prevDx || dy != prevDy)
        {
            result.push_back(prevNode);
        }

        prevDx = dx;
        prevDy = dy;
    }

    result.push_back(nodes.back());

    return result;
}

PathNode* ConvertPath(const std::vector<std::shared_ptr<PathNodeInterface>> path)
{
    PathNode* nodes = new PathNode[path.size()];

    for (size_t i = 0; i < path.size(); i++) {
        const auto& node = path[i];

        const auto& minCoords = Converter::CellCoordsToWorld(node->GetCoords());
        nodes[i] = {
            minCoords,
            { minCoords.x + Constants::CELL_SIZE_IN_GAME_COORDS, minCoords.y + Constants::CELL_SIZE_IN_GAME_COORDS },
            node->GetHeight()
        };
    }

    return nodes;
}
