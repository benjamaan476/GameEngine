#include "BoardBuilder.h"

void BoardBuilder::new_rank() noexcept
{
	_board.emplace_back();
}

void BoardBuilder::set_piece(const int2& position, char piece)
{
	assert(_board.size() > position.x);
	auto& rank = _board.at(position.x);

	if (position.y >= rank.size())
	{
		rank.resize(position.y + 1);
	}
	rank[position.y] = piece;
}

void BoardBuilder::set_next_player(std::string_view player)
{
	if (player.compare("w") == 0)
	{
		_nextPlayer = Colour::White;
	}
	else if (player.compare("b") == 0)
	{
		_nextPlayer = Colour::Black;
	}
	else
	{
		assert(false);
	}
}

void BoardBuilder::set_enpassant_square(std::string_view square)
{
	assert(square.length() == 2);
	auto file = std::tolower(square[0]) - 'a';
	auto rank = square[1] - '0';
	_enpassant = { rank, file };
}

void BoardBuilder::set_halfmove(std::string_view halfmove)
{
	_halfmove = std::stoi(halfmove.data());
}

void BoardBuilder::set_fullmove(std::string_view fullmove)
{
	_fullmove = std::stoi(fullmove.data());
}

Board BoardBuilder::build() const noexcept
{
	return Board{ {8, 8}, nullptr };
};