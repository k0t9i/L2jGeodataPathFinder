#pragma once

#include <format>
#include <stdint.h>
#include <unordered_map>
#include <memory>
#include <functional>
#include "Layer.h"

class LayerFactory
{
public:
	LayerFactory() = default;
	virtual ~LayerFactory() = default;

	const std::shared_ptr<Layer> Create(const int16_t height, const bool north, const bool south, const bool west, const bool east)
	{
		const auto key = GetKey(height, north, south, west, east);
		if (m_Layers.find(key) == m_Layers.end()) {
			m_Layers[key] = std::make_shared<Layer>(height, north, south, west, east);
		}

		return m_Layers[key];
	}

private:
	using KeyType = std::tuple<int16_t, bool, bool, bool, bool>;
	const std::size_t GetKey(const int16_t height, const bool north, const bool south, const bool west, const bool east) const
	{
		return hashValue(std::make_tuple(height, north, south, west, east));
	}

	template<typename T>
	const std::size_t hashValue(T const& value) const
	{
		std::hash<T> hasher;
		return hasher(value);
	}

	template<>
	const std::size_t hashValue<KeyType>(KeyType const& value) const
	{
		std::size_t result = 0;
		std::apply(
			[&result, this](auto&&... args) {
				((result ^= hashValue(args) + 0x9e3779b9 + (result << 6) + (result >> 2)), ...);
			},
			value
		);
		return result;
	}

private:
	std::unordered_map<std::size_t, std::shared_ptr<Layer>> m_Layers;
};