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

class Board
{
public:
	Board(const int2& size, std::unique_ptr<BoardRenderer>&& renderer) noexcept;

	void draw() const noexcept;

	Bitboard get_pieces() const noexcept;
	Bitboard get_white_pieces() const noexcept;
	Bitboard get_black_pieces() const noexcept;

	void set_next_player(Colour player) noexcept;

private:
	int2 _size{};
	std::unique_ptr<BoardRenderer> _renderer;

	Bitboard _whitePawns;
	Bitboard _whiteRooks;
	Bitboard _whiteKnights;
	Bitboard _whiteBishops;
	Bitboard _whiteKing;
	Bitboard _whiteQueens;
	Bitboard _blackPawns;
	Bitboard _blackRooks;
	Bitboard _blackKnights;
	Bitboard _blackBishops;
	Bitboard _blackKing;
	Bitboard _blackQueens;

	std::vector<Bitboard> _whitePieces;

	Colour _nextPlayer;
};

