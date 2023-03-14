module;

#include "../GameEngine/EngineCore.h"
#include <random>

export module MagicNumberGenerator;

import Bitboard;
import MovementSet;

export class MagicNumberGenerator
{
public:
	inline uint64_t generateRandomU64() const
	{
		return mt();
	}

	Bitboard generateRandomBitboard(uint2 size)
	{
		auto numberWords = ((size.x * size.y - 1) >> 6) + 1;
		std::vector<uint64_t> words;
		for (auto i = 0u; i < numberWords; i++)
		{
			words.emplace_back(generateRandomU64() & generateRandomU64() & generateRandomU64());
		}
		return Bitboard(size, words);
	}

	MagicNumberGenerator()
	{
		mt.seed(0);
	}

	void findMagicNumber(uint2 size, int square, const std::vector<Bitboard>& pieceMovementSet)
	{
		std::vector<Bitboard> occupancies{};
		occupancies.resize(4096);

		std::vector<Bitboard> attacks{};
		attacks.resize(4096);

		std::vector<Bitboard> usedAttacks{};
		usedAttacks.resize(4096);

		auto& attackMask = pieceMovementSet[square];
		auto popCount = attackMask.bitCount();

		for (auto i = 0; i < popCount; i++)
		{
			occupancies[i] = MovementSet::setOccupancies(size, i, attackMask);
		}


	}

private:
	static inline std::mt19937_64 mt{};
};