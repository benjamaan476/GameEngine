#pragma once

#include "../GameEngine/EngineCore.h"

#include "Bitboard.h"

#include <random>

class MagicNumberGenerator
{
public:

	MagicNumberGenerator() noexcept;

	void find_magic_number(uint2 size, int square, const std::vector<Bitboard>& pieceMovementSet) noexcept;

private:
	inline uint64_t generate_random_u64() const noexcept;
	constexpr Bitboard generate_random_bitboard(uint2 size);

private:
	static inline std::mt19937_64 mt{};
};