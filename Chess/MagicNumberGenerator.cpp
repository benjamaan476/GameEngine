#include "MagicNumberGenerator.h"

#include "MovementSet.h"

MagicNumberGenerator::MagicNumberGenerator() noexcept
{
	mt.seed(0);
}

inline uint64_t MagicNumberGenerator::generate_random_u64() const noexcept
{
	return mt();
}

constexpr Bitboard MagicNumberGenerator::generate_random_bitboard(uint2 size)
{
	auto numberWords = ((size.x * size.y - 1) >> 6) + 1;
	std::vector<uint64_t> words;
	for (auto i = 0u; i < numberWords; i++)
	{
		words.emplace_back(generate_random_u64() & generate_random_u64() & generate_random_u64());
	}
	return Bitboard(size, words);
}

void MagicNumberGenerator::find_magic_number(uint2 size, int square, const std::vector<Bitboard>& pieceMovementSet) noexcept
{
	std::vector<Bitboard> occupancies{};
	occupancies.resize(4096);

	std::vector<Bitboard> attacks{};
	attacks.resize(4096);

	std::vector<Bitboard> usedAttacks{};
	usedAttacks.resize(4096);

	auto& attackMask = pieceMovementSet[square];
	auto popCount = attackMask.bit_count();

	for (auto i = 0; i < popCount; i++)
	{
		occupancies[i] = MovementSet::set_occupancies(size, i, attackMask);
	}


}
