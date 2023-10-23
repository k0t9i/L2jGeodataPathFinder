#pragma once

#include <cstdint>

class Constants
{
public:
	const static uint8_t CELLS_DIMENSION = 8;
	const static uint16_t BLOCKS_DIMENSION = 256;
	const static uint32_t REGION_CELLS_DIMENSION = BLOCKS_DIMENSION * CELLS_DIMENSION;
	const static uint8_t DELTA_COL = 20;
	const static uint8_t DELTA_ROW = 18;
	const static uint16_t BLOCK_SIZE_IN_GAME_COORDS = 32768;
	const static uint16_t CELL_SIZE_IN_GAME_COORDS = BLOCK_SIZE_IN_GAME_COORDS / REGION_CELLS_DIMENSION;
};