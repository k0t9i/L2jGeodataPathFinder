#pragma once

#include <memory>
#include "PathNodeInterface.h"
#include "Point.h"

class PathNodeFactoryInterface
{
public:
	PathNodeFactoryInterface() = default;
	virtual ~PathNodeFactoryInterface() = default;

	virtual const std::shared_ptr<PathNodeInterface> CreateNode(const float x, const float y, const float z) = 0;
	virtual const std::shared_ptr<PathNodeInterface> CreateNode(const Point& coords, const float z) = 0;
	virtual const std::shared_ptr<PathNodeInterface> CreateStartNode(const float x, const float y, const float z) = 0;
	virtual const std::shared_ptr<PathNodeInterface> CreateTargetNode(const float x, const float y, const float z) = 0;
};