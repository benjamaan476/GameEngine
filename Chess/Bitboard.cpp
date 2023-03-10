#include "Bitboard.h"

#include "../GameEngine/EngineCore.h"

#include <iostream>


constexpr Bitboard::Bitboard(uint32_t width, uint32_t height) noexcept : width{ width }, height{ height }
{
	words.resize(((width * height - 1) >> bitsPerWord) + 1);
	size = width * height;
	if ((size & wordSizeMask) == 0)
	{
		partialMask = ~0ull;
	}
	else
	{
		partialMask = (1ull << (size & wordSizeMask)) - 1;
	}
}

constexpr Bitboard::Bitboard(const Bitboard& board) noexcept
{
	width = board.width;
	height = board.height;
	size = board.size;
	partialMask = board.partialMask;
	words = board.words;
}

Bitboard::Bitboard(uint32_t width, uint32_t height, std::vector<uint64_t> word) noexcept
	: Bitboard{width, height}
{
	words = word;
	words.back() &= partialMask;
}

void Bitboard::setSquare(uint32_t index) noexcept
{
	if (index >= size)
	{
		return;
	}
	words[index >> bitsPerWord] |= 1ull << (index & wordSizeMask);
}

void Bitboard::setSquare(uint32_t x, uint32_t y) noexcept
{
	setSquare(x * width + y);
}

void Bitboard::unsetSquare(uint32_t index) noexcept
{
	if (index >= size)
	{
		return;
	}
	words[index >> bitsPerWord] &= ~(1ull << (index & wordSizeMask));
}

void Bitboard::unsetSquare(uint32_t x, uint32_t y) noexcept
{
	unsetSquare(x * width + y);
}

void Bitboard::toggleSquare(uint32_t index) noexcept
{
	if (index >= size)
	{
		return;
	}
	words[index >> bitsPerWord] ^= 1ull << (index & wordSizeMask);
}

constexpr int Bitboard::bitCount() const noexcept
{
	auto count = 0;
	for (const auto& word : words)
	{
		count += std::popcount(word);
	}
	return count;
}

Bitboard Bitboard::fillFile(uint32_t file) noexcept
{
	if (file < width)
	{
		for (auto i = 0u; i < width * height; i += width)
		{
			auto index = i + file;
			words[index >> bitsPerWord] |= 1ull << (index & wordSizeMask);
		}
	}
	return *this;
}

Bitboard Bitboard::fillRank(uint32_t rank) noexcept
{
	if (rank < height)
	{
		auto start = rank * width;
		auto end = start + width;

		for (auto i = start; i < end; i++)
		{
			words[i >> bitsPerWord] |= 1ull << (i & wordSizeMask);
		}
	}
	return *this;
}

Bitboard operator&(Bitboard lhs, const Bitboard& rhs) noexcept
{
	lhs &= rhs;
	return lhs;
}

Bitboard operator|(Bitboard lhs, const Bitboard& rhs) noexcept
{
	lhs |= rhs;
	return lhs;
}

Bitboard operator^(Bitboard lhs, const Bitboard& rhs) noexcept
{
	lhs ^= rhs;
	return lhs;
}


Bitboard Bitboard::operator&=(const Bitboard& board) noexcept
{
	assert(size == board.size);

	for (auto i = 0u; i < words.size(); i++)
	{
		words[i] &= board.words[i];
	}
	words.back() &= partialMask;

	return *this;
}

Bitboard Bitboard::operator|=(const Bitboard& board) noexcept
{
	assert(size == board.size);

	for (auto i = 0u; i < words.size(); i++)
	{
		words[i] |= board.words[i];
	}
	words.back() &= partialMask;

	return *this;
}

Bitboard Bitboard::operator^=(const Bitboard& board) noexcept
{
	assert(size == board.size);

	for (auto i = 0u; i < words.size(); i++)
	{
		words[i] ^= board.words[i];
	}

	words.back() &= partialMask;

	return *this;
}

Bitboard Bitboard::ls1b() const noexcept
{
	uint64_t ls1b{};
	int count = 0;
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

	Bitboard board{ width, height };

	board.words[count] = ls1b;

	return board;
}

int Bitboard::ls1bIndex() const noexcept
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

Bitboard Bitboard::operator~()
{
	Bitboard board(width, height);

	for (auto i = 0u; i < words.size(); i++)
	{
		board.words[i] = ~words[i];
	}
	board.words.back() &= partialMask;

	return board;
}

void Bitboard::draw() const noexcept
{
	std::ostringstream ss;

	uint32_t count = 0u;
	for (auto index = 0u; index < size; index += width)
	{
		auto word = words[index >> bitsPerWord];

		auto shift = index;

		auto max = ~0ull;

		max >>= (wordSize - width);
		auto byte = word & (max << shift);
		byte >>= shift;

		if (index >> bitsPerWord != (index + width - 1) >> bitsPerWord)
		{
			auto nextWord = words[(index + width - 1) >> bitsPerWord];
			auto nextIndex = index + width - wordSize;
			auto j = ~0ull >> (wordSize - nextIndex);
			auto nextByte = nextWord & (j/* << nextIndex*/);
			//nextByte >>= nextIndex;

			auto space = wordSize - index;

			byte |= (nextByte << space);
		}

		//byte &= 0xff;
		for (auto i = 0u; i < width; i++)
		{
			ss << ((byte >> i) & 0x01);
			count++;

			if ((count) % width == 0)
			{
				ss << "\n";
				count = 0;
			}

		}

	}

	std::cout << ss.str() << std::endl;
}

