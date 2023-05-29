#include "Board.h"

#include <functional>
#include <numeric>

Board::Board(const int2& size, std::unique_ptr<BoardRenderer>&& renderer) noexcept
	:
	_size{ size },
	_renderer{ std::move(renderer) },
	_whitePawns{ size },
	_whiteRooks{ size },
	_whiteKnights{ size },
	_whiteBishops{ size },
	_whiteKing{ size },
	_whiteQueens{ size },
	_blackPawns{ size },
	_blackRooks{ size },
	_blackKnights{ size },
	_blackBishops{ size },
	_blackKing{ size },
	_blackQueens{ size }
{
	_whitePawns.fill_file(size.y - 1);
	_whitePieces.push_back(_whitePawns);
	_whitePieces.push_back(_whiteRooks);
	_whitePieces.push_back(_whiteKnights);
	_whitePieces.push_back(_whiteBishops);
	_whitePieces.push_back(_whiteQueens);
	_whitePieces.push_back(_whiteKing);
}

void Board::draw() const noexcept
{
	_renderer->draw(*this);
}

Bitboard Board::get_white_pieces() const noexcept
{
	auto answer = std::accumulate(_whitePieces.begin(), _whitePieces.end(), _whitePawns, std::bit_or<Bitboard>());
	return _whitePawns | _whiteRooks | _whiteKnights | _whiteBishops | _whiteQueens | _whiteKing;
}

Bitboard Board::get_black_pieces() const noexcept
{
	return _blackPawns | _blackRooks | _blackKnights | _blackBishops | _blackQueens | _blackKing;
}

void Board::set_next_player(Colour player) noexcept
{
	_nextPlayer = player;
}

Bitboard Board::get_pieces() const noexcept
{
	return get_white_pieces() | get_black_pieces();
}
