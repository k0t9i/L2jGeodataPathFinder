# L2jGeodataPathFinder
Path finder in L2j Geodata files using A* search algorithm.
## Repository structure
#### Path Finder
Dynamic link library exporting two methods.

Finds path by the game coords. Fills first argument with array of path node metadata (height and minX, maxX, minY, maxY in game coords) and returns size of this array.
If onlyTurningPoints = true, then a path containing only root nodes (path turning points) will be returned:
```cpp
size_t FindPath(PathNode ** result, const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, const uint16_t maxPassableHeight, const bool onlyTurningPoints = false);
```
Releases memory for a previously found path:
```cpp
void ReleasePath(PathNode * path);
```

#### Usage Example
Console C# application with usage example.
Finds path from (80364, 147100, -3533) (near the Giran weapons store) to (83864, 143100) (north bridge of Giran)

Visualization of this example (height map and image generation done in another application):
![output](https://github.com/k0t9i/L2jGeodataPathFinder/assets/7733997/825cdb90-adc1-4ed2-a9ea-7d841f139e9c)

