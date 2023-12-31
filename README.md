# L2jGeodataPathFinder
Path finder in L2j Geodata files using A* search algorithm with post smoothing.
## Repository structure
#### Path Finder
Dynamic link library exporting two methods.

Finds path by the game coords. Fills first argument with array of path node metadata (height and minX, maxX, minY, maxY in game coords) and returns size of this array:
```cpp
size_t FindPath(PathNode ** result, const char* geoDataDir, const float startX, const float startY, const float startZ, const float endX, const float endY, const uint16_t maxPassableHeight);
```
Releases memory for a previously found path:
```cpp
void ReleasePath(PathNode * path);
```

#### Usage Example
Console C# application with usage example.
Finds path from (80364, 147100, -3533) (near the Giran weapons store) to (83864, 143100) (north bridge of Giran)

Visualization of this example, red - original A* path, yellow - smoothed path (height map and image generation done in another application):
![gh](https://github.com/k0t9i/L2jGeodataPathFinder/assets/7733997/dcc648a7-9355-4dc6-a6e8-471e823ca370)

Real life usage in a [ingame bot](https://github.com/k0t9i/L2Bot2.0):
![image_2023-10-29_20-53-56](https://github.com/k0t9i/L2jGeodataPathFinder/assets/7733997/4c54398b-cd25-40a2-ae7b-67c18efa4c3a)
