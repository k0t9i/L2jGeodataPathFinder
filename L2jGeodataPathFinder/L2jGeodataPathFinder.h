#ifdef L2JGEODATAPATHFINDER_EXPORTS
#define L2JGEODATAPATHFINDER_API __declspec(dllexport)
#else
#define L2JGEODATAPATHFINDER_API __declspec(dllimport)
#endif

#include <cstdint>
#include <vector>
#include <tuple>
#include <string>
#include "GeoData/Point.h"

struct PathNode
{
	Point min;
	Point max;
	int16_t z;
};

extern "C" L2JGEODATAPATHFINDER_API size_t FindPath(PathNode ** result, const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, const uint16_t maxPassableHeight);
extern "C" L2JGEODATAPATHFINDER_API void ReleasePath(PathNode * path);
