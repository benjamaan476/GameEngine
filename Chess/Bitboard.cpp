#include "Bitboard.h"

constexpr Bitboard::Bitboard(uint2 size) noexcept
	: _size{ size }, _flatSize{ size.x * size.y }
{
	_words.resize(((_flatSize - 1) >> bitsPerWord) + 1);
	if ((_flatSize & wordSizeMask) == 0)
	{
		_partialMask = ~0ull;
	}
	else
	{
		_partialMask = (1ull << (_flatSize & wordSizeMask)) - 1;
	}
}

constexpr Bitboard::Bitboard(const Bitboard& board) noexcept
	: Bitboard{ board._size }
{
	_partialMask = board._partialMask;
	_words = board._words;
}

constexpr Bitboard::Bitboard(uint2 size, std::vector<uint64_t> word) noexcept
	: Bitboard{ size }
{
	_words = word;
	_words.back() &= _partialMask;
}

bool Bitboard::is_set(uint32_t index) const noexcept
{
	return _words[index >> bitsPerWord] & 1ull << (index & wordSizeMask);
}

bool Bitboard::is_set(uint32_t x, uint32_t y) const noexcept
{
	return is_set(x + _size.x * y);
}

void Bitboard::set_square(uint32_t index) noexcept
{
	if (index >= _flatSize)
	{
		return;
	}
	_words[index >> bitsPerWord] |= 1ull << (index & wordSizeMask);
}

void Bitboard::set_square(uint2 square) noexcept
{
	set_square(square.y * _size.x + square.x);
}

void Bitboard::set_square(uint32_t x, uint32_t y) noexcept
{
	set_square(y * _size.x + x);
}

void Bitboard::unset_square(uint32_t index) noexcept
{
	if (index >= _flatSize)
	{
		return;
	}
	_words[index >> bitsPerWord] &= ~(1ull << (index & wordSizeMask));
}

void Bitboard::unset_square(uint2 square) noexcept
{
	unset_square(square.y * _size.x + square.x);
}

void Bitboard::unset_square(uint32_t x, uint32_t y) noexcept
{
	unset_square(x * _size.x + y);
}

void Bitboard::toggle_square(uint32_t index) noexcept
{
	if (index >= _flatSize)
	{
		return;
	}
	_words[index >> bitsPerWord] ^= 1ull << (index & wordSizeMask);
}

constexpr int Bitboard::bit_count() const noexcept
{
	auto count = 0;
	for (const auto& word : _words)
	{
		count += std::popcount(word);
	}
	return count;
}

void Bitboard::fill_file(uint32_t file) noexcept
{
	if (file < _size.x)
	{
		for (auto i = 0u; i < _flatSize; i += _size.x)
		{
			auto index = i + file;
			_words[index >> bitsPerWord] |= 1ull << (index & wordSizeMask);
		}
	}
}

void Bitboard::fill_rank(uint32_t rank) noexcept
{
	if (rank < _size.y)
	{
		auto start = rank * _size.x;
		auto end = start + _size.x;

		for (auto i = start; i < end; i++)
		{
			_words[i >> bitsPerWord] |= 1ull << (i & wordSizeMask);
		}
	}
}

Bitboard Bitboard::operator&=(const Bitboard& board) noexcept
{
	assert(_size == board._size);

	for (auto i = 0u; i < _words.size(); i++)
	{
		_words[i] &= board._words[i];
	}
	_words.back() &= _partialMask;

	return *this;
}

Bitboard Bitboard::operator|=(const Bitboard& board) noexcept
{
	assert(_size == board._size);

	for (auto i = 0u; i < _words.size(); i++)
	{
		_words[i] |= board._words[i];
	}
	_words.back() &= _partialMask;

	return *this;
}

Bitboard Bitboard::operator^=(const Bitboard& board) noexcept
{
	assert(_size == board._size);

	for (auto i = 0u; i < _words.size(); i++)
	{
		_words[i] ^= board._words[i];
	}

	_words.back() &= _partialMask;

	return *this;
}

Bitboard Bitboard::operator~()
{
	Bitboard board(_size);

	for (auto i = 0u; i < _words.size(); i++)
	{
		board._words[i] = ~_words[i];
	}

	board._words.back() &= _partialMask;

	return board;
}

Bitboard Bitboard::least_significant_first_bit() const noexcept
{
	uint64_t ls1b{};
	int count{};
	for (const auto& word : _words)
	{
		if (word == 0)
		{
			count++;
			continue;
		}

		ls1b = word & (0 - word);
		break;
	}

	Bitboard board{ _size };

	board._words[count] = ls1b;

	return board;
}

int Bitboard::least_significant_first_bit_index() const noexcept
{
	auto board = least_significant_first_bit();

	for (auto& word : board._words)
	{
		if (word != 0)
		{
			word -= 1;
			break;
		}
		word -= 1;
	}

	return board.bit_count();
}

void Bitboard::draw() const noexcept
{
	std::ostringstream ss;

	uint32_t count = 0u;
	for (auto index = 0u; index < _flatSize; index += _size.x)
	{
		auto word = _words[index >> bitsPerWord];

		auto shift = index;

		auto max = ~0ull;

		max >>= (wordSize - _size.x);
		auto byte = word & (max << shift);
		byte >>= shift;

		if (index >> bitsPerWord != (index + _size.x - 1) >> bitsPerWord)
		{
			auto nextWord = _words[(index + _size.x - 1) >> bitsPerWord];
			auto nextIndex = index + _size.x - wordSize;
			auto j = ~0ull >> (wordSize - nextIndex);
			auto nextByte = nextWord & (j/* << nextIndex*/);
			//nextByte >>= nextIndex;

			auto space = wordSize - index;

			byte |= (nextByte << space);
		}

		//byte &= 0xff;
		for (auto i = 0u; i < _size.x; i++)
		{
			ss << ((byte >> i) & 0x01);
			count++;

			if ((count) % _size.x == 0)
			{
				ss << "\n";
				count = 0;
			}
		}
	}
	std::cout << ss.str() << std::endl;
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
