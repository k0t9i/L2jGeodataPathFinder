#pragma once

#include <memory>
#include <tuple>
#include <map>
#include <format>
#include <cmath>
#include <shared_mutex>
#include "Loader.h"
#include "Region.h"
#include "Point.h"
#include "Converter.h"

class DataHandler
{
public:
	DataHandler() = default;
	virtual ~DataHandler() = default;

	const Region::Cell& GetCell(const Point& coords)
	{
		std::shared_lock<std::shared_timed_mutex>(m_Mutex);
		const auto& region = LoadRegion(Converter::CellCoordsToRegion(coords));

		return region->GetCell(coords);
	}

	void SetDataDirectory(const std::string& dataDirectory)
	{
		m_DataDirectory = dataDirectory;
	}

	const void UnloadRegionsExcept(const Point& startRegionCoords, const Point& targetRegionCoords)
	{
		for (auto it = m_Regions.cbegin(); it != m_Regions.cend();)
		{
			if (it->first.x == startRegionCoords.x && it->first.y == startRegionCoords.y || it->first.x == targetRegionCoords.x && it->first.y == targetRegionCoords.y)
			{
				++it;
			}
			else
			{
				it = m_Regions.erase(it);
			}
		}
	}

private:
	const std::unique_ptr<Region>& LoadRegion(const Point& coords)
	{
		if (m_Regions.find(coords) == m_Regions.end()) {
			// TODO check for existence
			auto region = m_Loader.LoadRegion(std::format("{}/{}_{}.l2j", m_DataDirectory, coords.x, coords.y), coords.x, coords.y);
			m_Regions[coords] = std::move(region);
		}

		return m_Regions[coords];
	}

private:
	std::string m_DataDirectory;
	std::map<Point, std::unique_ptr<Region>> m_Regions;
	Loader m_Loader;
	std::shared_timed_mutex m_Mutex;
};