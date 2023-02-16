#pragma once
#include <array>
#include <vector>

class Bitboard
{
public:
	Bitboard(uint32_t width, uint32_t height) noexcept;
	constexpr Bitboard(const Bitboard& board) noexcept;

	Bitboard fillFile(uint32_t file) noexcept;
	Bitboard fillRank(uint32_t rank) noexcept;

	friend Bitboard operator& (Bitboard lhs, const Bitboard& rhs) noexcept;
	Bitboard operator&= (const Bitboard& noard) noexcept;

	friend Bitboard operator| (Bitboard lhs, const Bitboard& rhs) noexcept;
	Bitboard operator|= (const Bitboard& board) noexcept;

	friend Bitboard operator^ (Bitboard lhs, const Bitboard& rhs) noexcept;
	Bitboard operator^= (const Bitboard& board) noexcept;

	Bitboard operator~ ();
	void draw() const noexcept;

private:

	constexpr static uint32_t bitsPerWord = 6;//  std::log2(sizeof(uint64_t)); log2 not constexpr?
	constexpr static uint32_t wordSize = 64; // sizeof(uint64_t);
	constexpr static uint64_t wordSizeMask = wordSize - 1;

	uint32_t width{};
	uint32_t height{};
	uint32_t size{};

	std::vector<uint64_t> words{};
	uint64_t partialMask{};
};