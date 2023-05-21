module;

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"

#include <functional>
#include <numeric>

export module Board;
import Bitboard;

class Board;
export class BoardRenderer
{
public:
	virtual void draw(const Board& board) const = 0;
};

export class Board
{
public:
	Board(const int2& size, std::unique_ptr<BoardRenderer>&& renderer) noexcept :
		size{ size },
		renderer{ std::move(renderer) },
		whitePawns{ size },
		whiteRooks{ size },
		whiteKnights{ size },
		whiteBishops{ size },
		whiteKing{ size },
		whiteQueens{ size },
		blackPawns{ size },
		blackRooks{ size },
		blackKnights{ size },
		blackBishops{ size },
		blackKing{ size },
		blackQueens{ size }
	{
		whitePawns.fillFile(size.y - 1);
		whitePieces.push_back(whitePawns);
		whitePieces.push_back(whiteRooks);
		whitePieces.push_back(whiteKnights);
		whitePieces.push_back(whiteBishops);
		whitePieces.push_back(whiteQueens);
		whitePieces.push_back(whiteKing);
	}

	void draw() const
	{
		renderer->draw(*this);
	}

	Bitboard getWhitePieces() const noexcept
	{
		auto answer = std::accumulate(whitePieces.begin(), whitePieces.end(), whitePawns, std::bit_or<Bitboard>());
		return whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
	}

	Bitboard getBlackPieces() const noexcept
	{
		return blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
	}

	Bitboard getPieces() const noexcept
	{
		return getWhitePieces() | getBlackPieces();
	}

	void set_next_player(Colour player) noexcept
	{
		_nextPlayer = player;
	}

private:
	int2 size{};
	std::unique_ptr<BoardRenderer> renderer;

	Bitboard whitePawns;
	Bitboard whiteRooks;
	Bitboard whiteKnights;
	Bitboard whiteBishops;
	Bitboard whiteKing;
	Bitboard whiteQueens;
	Bitboard blackPawns;
	Bitboard blackRooks;
	Bitboard blackKnights;
	Bitboard blackBishops;
	Bitboard blackKing;
	Bitboard blackQueens;

	std::vector<Bitboard> whitePieces;

	Colour _nextPlayer;
};

