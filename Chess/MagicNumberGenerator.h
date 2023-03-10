#include <random>
#include "Bitboard.h"

class MagicNumberGenerator
{
public:
	inline uint64_t generateRandomU64()
	{
		return mt();
	}

	Bitboard generateRandomBitboard(uint32_t width, uint32_t height)
	{
		auto numberWords = ((width * height - 1) >> 6) + 1;
		std::vector<uint64_t> words;
		for (auto i = 0u; i < numberWords; i++)
		{
			words.emplace_back( generateRandomU64() & generateRandomU64() & generateRandomU64());
		}
		return Bitboard{ width, height, words };
	}

	MagicNumberGenerator()
	{
		mt.seed(0);
	}

	template<uint32_t width, uint32_t height>
	Bitboard findMagicNumber(int square, const std::vector<Bitboard>& pieceMovementSet)
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
			occupancies[i] = MovementSet<width, height>::setOccupancies(i, attackMask);
		}


	}

private:
	static inline std::mt19937_64 mt{ };


};