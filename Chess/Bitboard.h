#pragma once


#include "../GameEngine/EngineCore.h"

#include <array>
#include <algorithm>

template<uint32_t width, uint32_t height>
class Bitboard
{
public:
	constexpr Bitboard() noexcept
	{
		if ((size & wordSizeMask) == 0)
		{
			partialMask = ~0;
		}
		else
		{
			partialMask = (1ULL << (size & wordSizeMask)) - 1;
		}
	}

	constexpr Bitboard(const Bitboard& board) noexcept
	{
		partialMask = board.partialMask;
		words = board.words;
	}
	constexpr Bitboard operator= (const Bitboard& board) noexcept
	{
		return { .words = board.words, .partialMask = board.partialMask };
	}


	static Bitboard fileMask(uint32_t file) noexcept
	{
		Bitboard<width, height> board;

		auto wordWidth = std::min(width, 8u);
		for (auto i = 0u; i < width * height; i += wordWidth)
		{
			auto index = i + file;
			board.words[index >> bitsPerWord] |= 1ull << (index & wordSizeMask);
		}

		return board;
	}

	void draw() const noexcept
	{
		std::ostringstream ss;

		//for (auto i = 0u; i < words.size(); i++)
		//{
		//	auto word = words[i];
		//	for (auto n = 0ull; n < 64; n++)
		//	{
		//		if (i == words.size() - 1)
		//		{
		//			if (((1ull << n) & partialMask) == 0)
		//			{
		//				continue;
		//			}
		//		}
		//		if (n % width == 0)
		//		{
		//			ss << "\n";

		//		}
		//		auto bit = word >> n;
		//		ss << bit;
		//	}
		//	ss << "\n";
		//}
		uint32_t count = 0u;
		for(auto index = 0u; index < size; index += wordWidth)
		{ 
			auto word = words[index >> bitsPerWord];

			auto shift = index;

			auto byte = word & (0xFFull << shift);
			byte >>= shift;
			byte &= 0xff;
			for (auto i = 0u; i < wordWidth; i++)
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
private:

	constexpr static uint32_t bitsPerWord = 6;//  std::log2(sizeof(uint64_t)); log2 not constexpr?
	constexpr static uint32_t wordSize = 64; // sizeof(uint64_t);
	constexpr static uint64_t wordSizeMask = wordSize - 1;

	const uint32_t size = width * height;
	const uint32_t wordWidth = std::min(width, 8u);
	//consteval const uint32_t numberOfWordsNeeded = size / wordSize;

	std::array<uint64_t, ((width * height - 1) >> bitsPerWord) + 1> words{};
	uint64_t partialMask{};
};