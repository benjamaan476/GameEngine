#pragma once

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"
#include "Board.h"

#include <list>
#include <vector>
#include <unordered_map>

class BoardBuilder
{
public:
	BoardBuilder();

	void new_rank() noexcept;

	void set_blank(int rank, int blank);
	void set_piece(int rank, char piece);
	void set_next_player(std::string_view player);
	void set_castling(std::string_view castling);
	void set_enpassant_square(std::string_view square);
	void set_halfmove(std::string_view halfmove);
	void set_fullmove(std::string_view fullmove);
	Board build() const noexcept;

private:
	void register_default_pieces();
	void register_custom_pieces();

private:
	std::vector<std::vector<int8_t>> _board{};
	Colour _nextPlayer{};
	std::optional<uint2> _enpassant{};
	int32_t _halfmove{};
	int32_t _fullmove{};

	std::unordered_map<int8_t, Piece> _pieceMap{};
};