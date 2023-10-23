#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <format>
#include "Region.h"
#include "LayerFactory.h"
#include "Constants.h"

class Loader
{
private:
	enum class BlockType : unsigned char
	{
		Simple = 0,
		Complex,
		Multilayer
	};

	struct NSWE
	{
		const bool N = true;
		const bool S = true;
		const bool W = true;
		const bool E = true;

		NSWE() = default;
		virtual ~NSWE() = default;

		NSWE(bool n, bool s, bool w, bool e) :
			N(n), S(s), W(w), E(e)
		{
		}
	};

public:
	const std::unique_ptr<Region> LoadRegion(const std::string& fileName, const uint8_t x, const uint8_t y)
	{
		const std::vector<uint8_t> bytes = LoadBytes(fileName);

		auto region = std::make_unique<Region>(x, y);

		for (uint32_t i = 0; i < bytes.size(); i++) {
			LoadBlock(region, static_cast<BlockType>(bytes[i]), bytes, i);
		}

		return region;
	}
private:
	const std::vector<uint8_t> LoadBytes(const std::string& fileName) const
	{
		std::ifstream input(fileName, std::ios::binary);
		std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
		input.close();

		return bytes;
	}

	const int16_t GetInfo(const std::vector<uint8_t>& bytes, const uint32_t startIndex) const
	{
		return (int16_t)(bytes[startIndex + 2] * 256 + bytes[startIndex + 1]);
	}
	const int16_t GetHeight(const int16_t info) const
	{
		return (int16_t)((int16_t)(info & 0xfff0) / 2);
	}
	const NSWE GetNSWE(const int16_t info) const
	{
		int16_t nswe = info & 0xf;

		return NSWE(
			(nswe & (1 << 3)) != 0,
			(nswe & (1 << 2)) != 0,
			(nswe & (1 << 1)) != 0,
			(nswe & (1 << 0)) != 0
		);
	}
	const Region::Cell CreateCell(const BlockType& type, const std::vector<uint8_t>& bytes, const uint16_t layersCount, uint32_t& index)
	{
		Region::Cell cell;
		for (size_t i = 0; i < layersCount; i++)
		{
			const auto info = GetInfo(bytes, index);
			const auto nswe = type != BlockType::Simple ? GetNSWE(info) : NSWE();
			const auto height = type != BlockType::Simple ? GetHeight(info) : info;

			cell.push_back(m_LayerFactory.Create(
				height,
				nswe.N,
				nswe.S,
				nswe.W,
				nswe.E
			));
			index += 2;
		}

		return cell;
	};
	void LoadBlock(std::unique_ptr<Region>& region, const BlockType& type, const std::vector<uint8_t>& bytes, uint32_t& index)
	{
		uint8_t cellCount = Constants::CELLS_DIMENSION * Constants::CELLS_DIMENSION;
		uint8_t layersCount = 1;

		if (type == BlockType::Simple) {
			const auto cell = CreateCell(type, bytes, layersCount, index);

			for (uint8_t i = 0; i < cellCount; i++)
			{
				region->AddCell(cell);
			}
		}
		else {
			for (uint8_t i = 0; i < cellCount; i++)
			{
				if (type == BlockType::Multilayer)
				{
					index++;
					layersCount = bytes[index];
				}
				region->AddCell(CreateCell(type, bytes, layersCount, index));
			}
		}
	}
	LayerFactory m_LayerFactory;
};