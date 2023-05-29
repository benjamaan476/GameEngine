#include "Board.h"

#include <functional>
#include <numeric>

Board::Board(const uint2& size, std::unique_ptr<BoardRenderer>&& renderer) noexcept
	:
	_size{ size },
	_renderer{ std::move(renderer) }
{
	register_base_pieces();
}

void Board::set_renderer(std::unique_ptr<BoardRenderer>&& renderer) noexcept
{
	_renderer = std::move(renderer);
}

void Board::draw() const noexcept
{
	if (_renderer)
	{
		_renderer->draw(*this);
	}
}

Bitboard Board::get_pieces() const noexcept
{
	return get_white_pieces() | get_black_pieces();
}

Bitboard Board::get_white_pieces() const noexcept
{
	const auto& white = _pieces.at(Colour::White);
	return std::accumulate(white.begin(), white.end(), Bitboard{ _size }, [](Bitboard&& lhs, std::pair<PieceType, Bitboard>&& rhs) { return lhs | rhs.second; });
}

Bitboard Board::get_black_pieces() const noexcept
{
	const auto& black = _pieces.at(Colour::Black);
	return std::accumulate(black.begin(), black.end(), Bitboard{ _size }, [](auto&& lhs, auto&& rhs) { return lhs | rhs.second; });
}

const Bitboard& Board::get_piece(Piece piece) const noexcept
{
	return _pieces.at(piece.colour).at(piece.type);
}

const Bitboard& Board::get_black_pawns() const noexcept
{
	return _pieces.at(Colour::Black).at(PieceType::Pawn);
}

void Board::set_next_player(Colour player) noexcept
{
	_nextPlayer = player;
}

void Board::register_base_pieces()
{
	_pieces.emplace(Colour::White, std::unordered_map<PieceType, Bitboard>{});

	auto& white = _pieces.at(Colour::White);
	white.emplace(PieceType::Pawn, Bitboard{_size});
	white.emplace(PieceType::Rook, Bitboard{_size});
	white.emplace(PieceType::Knight, Bitboard{_size});
	white.emplace(PieceType::Bishop, Bitboard{_size});
	white.emplace(PieceType::King, Bitboard{_size});
	white.emplace(PieceType::Queen, Bitboard{_size});

	_pieces.emplace(Colour::Black, std::unordered_map<PieceType, Bitboard>{});
	auto& black = _pieces.at(Colour::Black);
	black.emplace(PieceType::Pawn, Bitboard{_size});
	black.emplace(PieceType::Rook, Bitboard{_size});
	black.emplace(PieceType::Knight, Bitboard{_size});
	black.emplace(PieceType::Bishop, Bitboard{_size});
	black.emplace(PieceType::King, Bitboard{_size});
	black.emplace(PieceType::Queen, Bitboard{_size});
}	


