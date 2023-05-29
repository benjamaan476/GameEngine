#pragma once

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"



#include "Bitboard.h"

class Board;
class BoardRenderer
{
public:
	virtual void draw(const Board& board) const = 0;
};

class BoardBuilder;
class Board
{
	friend class BoardBuilder;
public:
	Board(const uint2& size, std::unique_ptr<BoardRenderer>&& renderer) noexcept;

	void set_renderer(std::unique_ptr<BoardRenderer>&& renderer) noexcept;
	uint2 get_size() const noexcept { return _size; }
	void draw() const noexcept;

	Bitboard get_pieces() const noexcept;
	Bitboard get_white_pieces() const noexcept;
	Bitboard get_black_pieces() const noexcept;

	const Bitboard& get_piece(Piece piece) const noexcept;
	
	
	const Bitboard& get_black_pawns() const noexcept;
	
	
	void set_next_player(Colour player) noexcept;

private:
	void register_base_pieces();

private:
	uint2 _size{};
	std::unique_ptr<BoardRenderer> _renderer;

	std::unordered_map<Colour, std::unordered_map<PieceType, Bitboard>> _pieces{};

	Colour _nextPlayer;
};

