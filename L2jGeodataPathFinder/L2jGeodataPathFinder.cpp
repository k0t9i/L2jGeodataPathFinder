#include "pch.h"
#include "framework.h"
#include "L2jGeodataPathFinder.h"
#include "GeoData/Pathfinder.h"
#include "GeoData/DataHandler.h"
#include "GeoData/LayerDecoratorFactory.h"
#include "GeoData/Converter.h"
#include <mutex>

DataHandler handler;
std::mutex handlerMutex;

extern "C" L2JGEODATAPATHFINDER_API size_t FindPath(PathNode ** result, const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, float range, const uint16_t maxPassableHeight)
{
    std::lock_guard<std::mutex> lock(handlerMutex);
    handler.SetDataDirectory(geoDataDir);
    LayerDecoratorFactory factory(handler, maxPassableHeight);
    Pathfinder pathfinder(factory);

    const auto path = pathfinder.FindPath(startX, startY, startZ, endX, endY, range);
    *result = path.empty() ? nullptr : ConvertPath(path);

    return path.size();
}

extern "C" L2JGEODATAPATHFINDER_API bool HasLineOfSight(const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, const float endZ)
{
    std::lock_guard<std::mutex> lock(handlerMutex);
    handler.SetDataDirectory(geoDataDir);
    LayerDecoratorFactory factory(handler, 9999);
    Pathfinder pathfinder(factory);

    return pathfinder.HasLineOfSight(startX, startY, startZ, endX, endY, endZ);
}

extern "C" L2JGEODATAPATHFINDER_API void ReleasePath(PathNode * path)
{
    delete[] path;
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
