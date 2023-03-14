module;

#include "../GameEngine/EngineCore.h"
#include <iostream>

export module Bitboard;

export class Bitboard
{
public:
	constexpr Bitboard() noexcept = default;
	constexpr Bitboard(uint2 size) noexcept : size{ size }, flatSize{size.x * size.y}
	{
		words.resize(((flatSize - 1) >> bitsPerWord) + 1);
		if ((flatSize & wordSizeMask) == 0)
		{
			partialMask = ~0ull;
		}
		else
		{
			partialMask = (1ull << (flatSize & wordSizeMask)) - 1;
		}
	}

	constexpr Bitboard(const Bitboard& board) noexcept
		: Bitboard{board.size}
	{
		partialMask = board.partialMask;
		words = board.words;
	}

	constexpr Bitboard(uint2 size, std::vector<uint64_t> word) noexcept
		: Bitboard{ size }
	{
		words = word;
		words.back() &= partialMask;
	}

	void setSquare(uint32_t index) noexcept
	{
		if (index >= flatSize)
		{
			return;
		}
		words[index >> bitsPerWord] |= 1ull << (index & wordSizeMask);
	}

	void setSquare(uint2 square) noexcept
	{
		setSquare(square.x * size.x + square.y);
	}

	void setSquare(uint32_t x, uint32_t y) noexcept
	{
		setSquare(x * size.x + y);
	}

	void unsetSquare(uint32_t index) noexcept
	{
		if (index >= flatSize)
		{
			return;
		}
		words[index >> bitsPerWord] &= ~(1ull << (index & wordSizeMask));
	}

	void unsetSquare(uint2 square) noexcept
	{
		unsetSquare(square.x * size.x + square.y);
	}

	void toggleSquare(uint32_t index) noexcept
	{
		if (index >= flatSize)
		{
			return;
		}
		words[index >> bitsPerWord] ^= 1ull << (index & wordSizeMask);
	}

	constexpr int bitCount() const noexcept
	{
		auto count = 0;
		for (const auto& word : words)
		{
			count += std::popcount(word);
		}
		return count;
	}

	Bitboard fillFile(uint32_t file) noexcept
	{
		if (file < size.x)
		{
			for (auto i = 0u; i < flatSize; i += size.x)
			{
				auto index = i + file;
				words[index >> bitsPerWord] |= 1ull << (index & wordSizeMask);
			}
		}
		return *this;
	}

	Bitboard fillRank(uint32_t rank) noexcept
	{
		if (rank < size.y)
		{
			auto start = rank * size.x;
			auto end = start + size.x;

			for (auto i = start; i < end; i++)
			{
				words[i >> bitsPerWord] |= 1ull << (i & wordSizeMask);
			}
		}
		return *this;
	}

	friend Bitboard operator&(Bitboard lhs, const Bitboard& rhs) noexcept
	{
		lhs &= rhs;
		return lhs;
	}

	friend Bitboard operator|(Bitboard lhs, const Bitboard& rhs) noexcept
	{
		lhs |= rhs;
		return lhs;
	}

	friend Bitboard operator^(Bitboard lhs, const Bitboard& rhs) noexcept
	{
		lhs ^= rhs;
		return lhs;
	}

	Bitboard operator&=(const Bitboard& board) noexcept
	{
		assert(size == board.size);

		for (auto i = 0u; i < words.size(); i++)
		{
			words[i] &= board.words[i];
		}
		words.back() &= partialMask;

		return *this;
	}

	Bitboard operator|=(const Bitboard& board) noexcept
	{
		assert(size == board.size);

		for (auto i = 0u; i < words.size(); i++)
		{
			words[i] |= board.words[i];
		}
		words.back() &= partialMask;

		return *this;
	}

	Bitboard operator^=(const Bitboard& board) noexcept
	{
		assert(size == board.size);

		for (auto i = 0u; i < words.size(); i++)
		{
			words[i] ^= board.words[i];
		}

		words.back() &= partialMask;

		return *this;
	}

	Bitboard operator~()
	{
		Bitboard board(size);

		for (auto i = 0u; i < words.size(); i++)
		{
			board.words[i] = ~words[i];
		}

		board.words.back() &= partialMask;

		return board;
	}

	Bitboard ls1b() const noexcept
	{
		uint64_t ls1b{};
		int count{};
		for (const auto& word : words)
		{
			if (word == 0)
			{
				count++;
				continue;
			}

			ls1b = word & (0 - word);
			break;
		}

		Bitboard board{ size };

		board.words[count] = ls1b;

		return board;
	}

	int ls1bIndex() const noexcept
	{
		auto board = ls1b();

		for (auto& word : board.words)
		{
			if (word != 0)
			{
				word -= 1;
				break;
			}
			word -= 1;
		}

		return board.bitCount();
	}

	void draw() const noexcept
	{
		std::ostringstream ss;

		uint32_t count = 0u;
		for (auto index = 0u; index < flatSize; index += size.x)
		{
			auto word = words[index >> bitsPerWord];

			auto shift = index;

			auto max = ~0ull;

			max >>= (wordSize - size.x);
			auto byte = word & (max << shift);
			byte >>= shift;

			if (index >> bitsPerWord != (index + size.x - 1) >> bitsPerWord)
			{
				auto nextWord = words[(index + size.x - 1) >> bitsPerWord];
				auto nextIndex = index + size.x - wordSize;
				auto j = ~0ull >> (wordSize - nextIndex);
				auto nextByte = nextWord & (j/* << nextIndex*/);
				//nextByte >>= nextIndex;

				auto space = wordSize - index;

				byte |= (nextByte << space);
			}

			//byte &= 0xff;
			for (auto i = 0u; i < size.x; i++)
			{
				ss << ((byte >> i) & 0x01);
				count++;

				if ((count) % size.x == 0)
				{
					ss << "\n";
					count = 0;
				}
			}
		}
		std::cout << ss.str() << std::endl;
	}

	constexpr uint2 getSize() const noexcept { return size; }
private:

	constexpr static uint32_t bitsPerWord = 6; // std::log2(8 * sizeof(uin64_t); log2 not constexpr?
	constexpr static uint32_t wordSize = 8 * sizeof(uint64_t); // 64 bits;
	constexpr static uint64_t wordSizeMask = wordSize - 1;

	uint2 size{};
	uint32_t flatSize{};
	std::vector<uint64_t> words{};
	uint64_t partialMask{};
};