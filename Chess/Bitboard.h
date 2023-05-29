#pragma once

#include "../GameEngine/EngineCore.h"

#include <iostream>

class Bitboard
{
public:
	constexpr Bitboard() noexcept = default;
	constexpr Bitboard(uint2 size) noexcept;
	constexpr Bitboard(const Bitboard& board) noexcept;
	constexpr Bitboard(uint2 size, std::vector<uint64_t> word) noexcept;

	~Bitboard() = default;

	bool is_set(uint32_t index) const noexcept;
	bool is_set(uint32_t x, uint32_t y) const noexcept;

	void set_square(uint32_t index) noexcept;
	void set_square(uint2 square) noexcept;
	void set_square(uint32_t x, uint32_t y) noexcept;

	void unset_square(uint32_t index) noexcept;
	void unset_square(uint2 square) noexcept;
	void unset_square(uint32_t x, uint32_t y) noexcept;

	void toggle_square(uint32_t index) noexcept;

	void fill_file(uint32_t file) noexcept;
	void fill_rank(uint32_t rank) noexcept;

	friend Bitboard operator&(Bitboard lhs, const Bitboard& rhs) noexcept;
	friend Bitboard operator|(Bitboard lhs, const Bitboard& rhs) noexcept;
	friend Bitboard operator^(Bitboard lhs, const Bitboard& rhs) noexcept;

	Bitboard operator&=(const Bitboard& board) noexcept;
	Bitboard operator|=(const Bitboard& board) noexcept;
	Bitboard operator^=(const Bitboard& board) noexcept;
	Bitboard operator~();

	constexpr int bit_count() const noexcept;
	Bitboard least_significant_first_bit() const noexcept;
	int least_significant_first_bit_index() const noexcept;

	void draw() const noexcept;

	constexpr uint2 get_size() const noexcept { return _size; }
private:

	constexpr static uint32_t bitsPerWord = 6; // std::log2(8 * sizeof(uin64_t); log2 not constexpr?
	constexpr static uint32_t wordSize = 8 * sizeof(uint64_t); // 64 bits;
	constexpr static uint64_t wordSizeMask = wordSize - 1;

	uint2 _size{};
	uint32_t _flatSize{};
	std::vector<uint64_t> _words{};
	uint64_t _partialMask{};
};
