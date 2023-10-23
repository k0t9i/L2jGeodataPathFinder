#pragma once

#include <memory>
#include <tuple>
#include <map>
#include <format>
#include <cmath>
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
		const auto& region = LoadRegion(Converter::CellCoordsToRegion(coords));

		return region->GetCell(coords);
	}

	void SetDataDirectory(const std::string& dataDirectory)
	{
		m_DataDirectory = dataDirectory;
	}

private:
	const std::shared_ptr<Region>& LoadRegion(const Point& coords)
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
	std::map<Point, std::shared_ptr<Region>> m_Regions;
	Loader m_Loader;
};