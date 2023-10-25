#ifdef L2JGEODATAPATHFINDER_EXPORTS
#define L2JGEODATAPATHFINDER_API __declspec(dllexport)
#else
#define L2JGEODATAPATHFINDER_API __declspec(dllimport)
#endif

#include <cstdint>
#include <vector>
#include <tuple>
#include <string>
#include <memory>
#include "GeoData/Point.h"
#include "GeoData/PathNodeInterface.h"

struct PathNode
{
	Point min;
	Point max;
	int16_t z;
};

extern "C" L2JGEODATAPATHFINDER_API size_t FindPath(PathNode ** result, const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, const uint16_t maxPassableHeight, const bool onlyTurningPoints = false);
extern "C" L2JGEODATAPATHFINDER_API void ReleasePath(PathNode * path);
PathNode* ConvertPath(const std::vector<std::shared_ptr<PathNodeInterface>> path);
const std::vector<std::shared_ptr<PathNodeInterface>> ReducePath(const std::vector<std::shared_ptr<PathNodeInterface>> nodes);
