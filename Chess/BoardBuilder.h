#pragma once

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"
#include "Board.h"

#include <vector>

class BoardBuilder
{
public:
	void new_rank() noexcept;

	void set_piece(const int2& position, char piece);
	void set_next_player(std::string_view player);
	void set_enpassant_square(std::string_view square);
	void set_halfmove(std::string_view halfmove);
	void set_fullmove(std::string_view fullmove);
	Board build() const noexcept;

private:
	std::vector<std::vector<int32_t>> _board{};
	Colour _nextPlayer{};
	std::optional<uint2> _enpassant{};
	int32_t _halfmove{};
	int32_t _fullmove{};
};