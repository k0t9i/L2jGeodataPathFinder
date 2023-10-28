#include "pch.h"
#include "framework.h"
#include "L2jGeodataPathFinder.h"
#include "GeoData/Pathfinder.h"
#include "GeoData/DataHandler.h"
#include "GeoData/LayerDecoratorFactory.h"
#include "GeoData/Converter.h"

DataHandler handler;

extern "C" L2JGEODATAPATHFINDER_API size_t FindPath(PathNode ** result, const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, const uint16_t maxPassableHeight)
{
    handler.SetDataDirectory(geoDataDir);
    LayerDecoratorFactory factory(handler, maxPassableHeight);
    Pathfinder pathfinder(factory);

    const auto path = pathfinder.FindPath(startX, startY, startZ, endX, endY);
    *result = ConvertPath(path);

    return path.size();
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
